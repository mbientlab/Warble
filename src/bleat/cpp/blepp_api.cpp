/**
 * @copyright MbientLab License
 */
#ifdef API_BLEPP

#include "gatt_def.h"
#include "gattchar_def.h"
#include "error_messages.h"

#include "blepp/blestatemachine.h"
#include "blepp/pretty_printers.h"

#include <condition_variable>
#include <fcntl.h>
#include <mutex>
#include <stdexcept>
#include <sys/time.h>
#include <thread>
#include <unistd.h>
#include <unordered_map>

using namespace std;
using namespace BLEPP;

struct BleatGattChar_Blepp;

struct BleatGatt_Blepp : public BleatGatt {
    BleatGatt_Blepp(const char* mac, const char* device);
    virtual ~BleatGatt_Blepp();

    virtual void connect_async(void* context, Void_VoidP_BleatGattP_CharP handler);
    virtual void disconnect();
    virtual void on_disconnect(void* context, Void_VoidP_BleatGattP_Uint handler);

    virtual BleatGattChar* find_characteristic(const std::string& uuid);

private:
    friend BleatGattChar_Blepp;
    
    const char *mac, *device;

    void *connect_context, *on_disconnect_context;
    Void_VoidP_BleatGattP_CharP connect_handler;
    Void_VoidP_BleatGattP_Uint on_disconnect_handler;

    BleatGattChar_Blepp* active_char;
    void* write_context;
    Void_VoidP_BleatGattCharP_CharP write_handler;

    BLEGATTStateMachine gatt;
    unordered_map<string, BleatGattChar_Blepp*> characteristics;

    condition_variable state_machine_cv;
    mutex cv_m;
    unique_lock<mutex> cv_lock;
    thread blepp_state_machine;
    bool terminate_state_machine;
};

struct BleatGattChar_Blepp : public BleatGattChar {
    BleatGattChar_Blepp(BleatGatt_Blepp* owner, BLEPP::Characteristic& ble_char);

    virtual ~BleatGattChar_Blepp();

    virtual void write_async(const std::uint8_t* value, std::uint8_t len, void* context, Void_VoidP_BleatGattCharP_CharP handler);
    virtual void write_without_resp_async(const std::uint8_t* value, std::uint8_t len, void* context, Void_VoidP_BleatGattCharP_CharP handler);

    virtual void read_async(void* context, Void_VoidP_BleatGattCharP_UbyteC_Ubyte_CharP handler);

    virtual void enable_notifications_async(void* context, Void_VoidP_BleatGattCharP_CharP handler);
    virtual void disable_notifications_async(void* context, Void_VoidP_BleatGattCharP_CharP handler);
    virtual void set_value_changed_handler(void* context, Void_VoidP_BleatGattCharP_UbyteC_Ubyte handler);

private:
    friend BleatGatt_Blepp;
    
    BleatGatt_Blepp* owner;
    Characteristic& ble_char;

    void *read_context, *value_changed_context;
    Void_VoidP_BleatGattCharP_UbyteC_Ubyte_CharP read_handler;
    Void_VoidP_BleatGattCharP_UbyteC_Ubyte value_changed_handler;

    function<void(const char*)> gatt_op_error_handler;
};

BleatGatt* bleatgatt_create(std::int32_t nopts, const BleatGattOption* opts) {
    const char *mac, *device;
    unordered_map<string, function<void(const char*)>> arg_processors = {
        {"mac", [&mac](const char* value) { mac = value; }}, 
        {"hci", [&device](const char* value) { device = value; }},
        {"log-level", [](const char* value) {
            if (!strcmp(value, "error")) {
                log_level = Error;
            } else if (!strcmp(value, "warning")) {
                log_level = Warning;
            } else if (!strcmp(value, "info")) {
                log_level = Info;
            } else if (!strcmp(value, "debug")) {
                log_level = Debug;
            } else if (!strcmp(value, "trace")) {
                log_level = Trace;
            }
        }}
    };

    for(int i = 0; i < nopts; i++) {
        auto it = arg_processors.find(opts[i].key);
        if (it == arg_processors.end()) {
            throw runtime_error(string("option '") + opts[i].key + "' does not exist");
        }
        (it->second)(opts[i].value);
    }
    if (mac == nullptr) {
        throw runtime_error("option 'mac' was not set");
    }

    return new BleatGatt_Blepp(mac, device);
}

BleatGatt_Blepp::BleatGatt_Blepp(const char* mac, const char* device) : mac(mac), device(device), cv_lock(cv_m), terminate_state_machine(true) {
    gatt.cb_connected = [this]() {
        gatt.read_primary_services();
    };
    gatt.cb_disconnected = [this](BLEGATTStateMachine::Disconnect d) {
        if (active_char != nullptr && active_char->gatt_op_error_handler != nullptr) {
            active_char->gatt_op_error_handler(BLEGATTStateMachine::get_disconnect_string(d));
        }

        auto copy = terminate_state_machine;
        terminate_state_machine = true;
        if (!copy) {
            on_disconnect_handler(on_disconnect_context, this, d.error_code);
        }
    };
    gatt.cb_find_characteristics = [this]() {
        gatt.get_client_characteristic_configuration();
    };
    gatt.cb_services_read = [this]() {
        gatt.find_all_characteristics();
    };
    gatt.cb_get_client_characteristic_configuration = [this]() {
        for(auto& service: gatt.primary_services) {
	        for(auto& characteristic: service.characteristics) {
                characteristics.emplace(to_str(characteristic.uuid), new BleatGattChar_Blepp(this, characteristic));
            }
        }

        connect_handler(connect_context, this, nullptr);
    };
    gatt.cb_write_response = [this]() {
        write_handler(write_context, active_char, nullptr);
    };

    thread th([this]() {
        auto socket_select = [this](timeval* timeout) {
            fd_set write_set, read_set;
            FD_ZERO(&read_set);
            FD_ZERO(&write_set);

            FD_SET(gatt.socket(), &read_set);
            if(gatt.wait_on_write()) {
                FD_SET(gatt.socket(), &write_set);
            }

            int status = select(gatt.socket() + 1, &read_set, &write_set, nullptr, timeout);
            if (status > 0) {
                if(FD_ISSET(gatt.socket(), &write_set)) {
                    gatt.write_and_process_next();
                }

                if(FD_ISSET(gatt.socket(), &read_set)) {
                    gatt.read_and_process_next();
                }
            }

            return status;
        };

        while(true) {
            state_machine_cv.wait(cv_lock, [this] { return gatt.socket() != -1; });

            timeval connect_timeout = { 10, 0 };
            int status = socket_select(&connect_timeout);
            if (status <= 0) {
                terminate_state_machine = true;
                gatt.close();
                connect_handler(connect_context, this, status == 0 ? BLEAT_CONNECT_TIMEOUT : BLEAT_GATT_ERROR);
            } else {
                terminate_state_machine = false;
                while(!terminate_state_machine && socket_select(nullptr) > 0) {
                }
            }
        }
    });
    swap(blepp_state_machine, th);
}

BleatGatt_Blepp::~BleatGatt_Blepp() {
    gatt.close();

    for(auto it: characteristics) {
        delete it.second;
    }
}

void BleatGatt_Blepp::connect_async(void* context, Void_VoidP_BleatGattP_CharP handler) {
    connect_context = context;
    connect_handler = handler;
    gatt.connect(mac, false, false, device == nullptr ? "" : device);
    state_machine_cv.notify_all();
}

void BleatGatt_Blepp::disconnect() {
    gatt.close();
}

void BleatGatt_Blepp::on_disconnect(void* context, Void_VoidP_BleatGattP_Uint handler) {
    on_disconnect_context = context;
    on_disconnect_handler = handler;
}

BleatGattChar* BleatGatt_Blepp::find_characteristic(const std::string& uuid) {
    return characteristics.at(uuid);
}

BleatGattChar_Blepp::BleatGattChar_Blepp(BleatGatt_Blepp* owner, BLEPP::Characteristic& ble_char) : owner(owner), ble_char(ble_char) {
    ble_char.cb_read = [this](const PDUReadResponse& r) {
        read_handler(read_context, this, r.value().first, r.value().second - r.value().first, nullptr);
    };
    ble_char.cb_notify_or_indicate = [this](const PDUNotificationOrIndication& n) {
        value_changed_handler(value_changed_context, this, n.value().first, n.value().second - n.value().first);
    };
}

BleatGattChar_Blepp::~BleatGattChar_Blepp() {

}

void BleatGattChar_Blepp::write_async(const uint8_t* value, uint8_t len, void* context, Void_VoidP_BleatGattCharP_CharP handler) {
    try {
        gatt_op_error_handler = [this, context, handler](const char* msg) {
            owner->active_char = nullptr;

            if (msg != nullptr) {
                stringstream error_stream;
                string full_msg;

                error_stream << BLEAT_GATT_WRITE_ERROR << "(" << msg << ")";
                full_msg = error_stream.str();
                handler(context, this, full_msg.c_str());
            } else {
                handler(context, this, msg);
            }            
        };
        
        owner->active_char = this;
        owner->write_context = context;
        owner->write_handler = handler;
        
        ble_char.write_request(value, len);
    } catch (const std::exception& e) {
        gatt_op_error_handler(e.what());
    } catch (const BLEDevice::WriteError&) {
        gatt_op_error_handler(nullptr);
    }
}

void BleatGattChar_Blepp::write_without_resp_async(const uint8_t* value, uint8_t len, void* context, Void_VoidP_BleatGattCharP_CharP handler) {
    stringstream error_stream;
    string msg;
    const char* error_msg = nullptr;

    try {
        ble_char.write_command(value, len);
    } catch (const std::exception& e) {
        error_stream << BLEAT_GATT_WRITE_ERROR << "(" << e.what() << ")";
        msg = error_stream.str();
        error_msg = msg.c_str();
    } catch (const BLEDevice::WriteError&) {
        error_msg = BLEAT_GATT_WRITE_ERROR;
    }

    handler(context, this, error_msg);
}

void BleatGattChar_Blepp::read_async(void* context, Void_VoidP_BleatGattCharP_UbyteC_Ubyte_CharP handler) {
    try {
        gatt_op_error_handler = [this, context, handler](const char* msg) {
            owner->active_char = nullptr;

            if (msg != nullptr) {
                stringstream error_stream;
                string full_msg;

                error_stream << BLEAT_GATT_READ_ERROR << "(" << msg << ")";
                full_msg = error_stream.str();
                handler(context, this, nullptr, 0, full_msg.c_str());
            } else {
                handler(context, this, nullptr, 0, msg);
            }            
        };

        owner->active_char = this;
        read_context = context;
        read_handler = handler;

        ble_char.read_request();
    } catch (const std::exception& e) {
        gatt_op_error_handler(e.what());
    } catch (const BLEDevice::WriteError&) {
        gatt_op_error_handler(nullptr);
    }
}

void BleatGattChar_Blepp::enable_notifications_async(void* context, Void_VoidP_BleatGattCharP_CharP handler) {
    try {
        gatt_op_error_handler = [this, context, handler](const char* msg) {
            owner->active_char = nullptr;

            if (msg != nullptr) {
                stringstream error_stream;
                string full_msg;

                error_stream << BLEAT_GATT_ENABLE_NOTIFY_ERROR << "(" << msg << ")";
                full_msg = error_stream.str();
                handler(context, this, full_msg.c_str());
            } else {
                handler(context, this, msg);
            }            
        };

        owner->active_char = this;
        owner->write_context = context;
        owner->write_handler = handler;
        ble_char.set_notify_and_indicate(true, false);
    } catch (const std::exception& e) {
        gatt_op_error_handler(e.what());
    }
}

void BleatGattChar_Blepp::disable_notifications_async(void* context, Void_VoidP_BleatGattCharP_CharP handler) {
    try {
        gatt_op_error_handler = [this, context, handler](const char* msg) {
            owner->active_char = nullptr;

            if (msg != nullptr) {
                stringstream error_stream;
                string full_msg;

                error_stream << BLEAT_GATT_DISABLE_NOTIFY_ERROR << "(" << msg << ")";
                full_msg = error_stream.str();
                handler(context, this, full_msg.c_str());
            } else {
                handler(context, this, msg);
            }            
        };

        owner->active_char = this;
        owner->write_context = context;
        owner->write_handler = handler;
        ble_char.set_notify_and_indicate(false, false);
    } catch (const std::exception& e) {
        gatt_op_error_handler(e.what());
    }
}

void BleatGattChar_Blepp::set_value_changed_handler(void* context, Void_VoidP_BleatGattCharP_UbyteC_Ubyte handler) {
    value_changed_context = context;
    value_changed_handler = handler;
}

#endif
