/**
 * @copyright MbientLab License
 */
#ifdef API_BLEPP

#include "gatt_def.h"
#include "gattchar_def.h"
#include "error_messages.h"

#include "blepp_utils.h"
#include "blepp/blestatemachine.h"
#include "blepp/pretty_printers.h"

#include <fcntl.h>
#include <stdexcept>
#include <sys/time.h>
#include <thread>
#include <unistd.h>
#include <unordered_map>
#include <unordered_set>

using namespace std;
using namespace BLEPP;

struct WarbleGattChar_Blepp;

struct WarbleGatt_Blepp : public WarbleGatt {
    WarbleGatt_Blepp(const char* mac, const char* hci_mac, bool public_addr);
    virtual ~WarbleGatt_Blepp();

    virtual void connect_async(void* context, FnVoid_VoidP_WarbleGattP_CharP handler);
    virtual void disconnect();
    virtual void on_disconnect(void* context, FnVoid_VoidP_WarbleGattP_Int handler);
    virtual bool is_connected() const;

    virtual WarbleGattChar* find_characteristic(const std::string& uuid) const;
    virtual bool service_exists(const std::string& uuid) const;

private:
    friend WarbleGattChar_Blepp;

    void clear_characteristics();

    string mac, hci_mac;

    void *on_disconnect_context;
    FnVoid_VoidP_WarbleGattP_Int on_disconnect_handler;

    WarbleGattChar_Blepp* active_char;
    void* write_context;
    FnVoid_VoidP_WarbleGattCharP_CharP write_handler;

    BLEGATTStateMachine gatt;
    unordered_map<string, WarbleGattChar_Blepp*> characteristics;
    unordered_set<string> services;

    thread blepp_state_machine;
    bool public_addr, connected, local_dc;
};

struct WarbleGattChar_Blepp : public WarbleGattChar {
    WarbleGattChar_Blepp(WarbleGatt_Blepp* owner, BLEPP::Characteristic& ble_char);

    virtual ~WarbleGattChar_Blepp();

    virtual void write_async(const std::uint8_t* value, std::uint8_t len, void* context, FnVoid_VoidP_WarbleGattCharP_CharP handler);
    virtual void write_without_resp_async(const std::uint8_t* value, std::uint8_t len, void* context, FnVoid_VoidP_WarbleGattCharP_CharP handler);

    virtual void read_async(void* context, FnVoid_VoidP_WarbleGattCharP_UbyteP_Ubyte_CharP handler);

    virtual void enable_notifications_async(void* context, FnVoid_VoidP_WarbleGattCharP_CharP handler);
    virtual void disable_notifications_async(void* context, FnVoid_VoidP_WarbleGattCharP_CharP handler);
    virtual void on_notification_received(void* context, FnVoid_VoidP_WarbleGattCharP_UbyteP_Ubyte handler);

    virtual const char* get_uuid() const;
    virtual WarbleGatt* get_gatt() const;
private:
    friend WarbleGatt_Blepp;
    
    WarbleGatt_Blepp* owner;
    Characteristic& ble_char;
    char uuid_str[37];

    void *read_context, *value_changed_context;
    FnVoid_VoidP_WarbleGattCharP_UbyteP_Ubyte_CharP read_handler;
    FnVoid_VoidP_WarbleGattCharP_UbyteP_Ubyte value_changed_handler;

    function<void(const char*)> gatt_op_error_handler;
};

WarbleGatt* warblegatt_create(std::int32_t nopts, const WarbleOption* opts) {
    const char *mac = nullptr, *hci_mac = "";
    bool public_addr = false;
    unordered_map<string, function<void(const char*)>> arg_processors = {
        {"mac", [&mac](const char* value) { mac = value; }}, 
        {"hci", [&hci_mac](const char* value) { hci_mac = value; }},
        {"address-type", [&public_addr](const char* value) {
            if (!strcmp(value, "public")) {
                public_addr = true;
            } else if (strcmp(value, "random")) {
                throw runtime_error("invalid value for \'address-type\' option (blepp api): one of [public, random]");
            }
        }},
    };

    for(int i = 0; i < nopts; i++) {
        auto it = arg_processors.find(opts[i].key);
        if (it == arg_processors.end()) {
            throw runtime_error(string("invalid gatt option '") + opts[i].key + "'");
        }
        (it->second)(opts[i].value);
    }
    if (mac == nullptr) {
        throw runtime_error("required option 'mac' was not set");
    }

    return new WarbleGatt_Blepp(mac, hci_mac, public_addr);
}

WarbleGatt_Blepp::WarbleGatt_Blepp(const char* mac, const char* hci_mac, bool public_addr) : 
        mac(mac), hci_mac(hci_mac), on_disconnect_context(nullptr), on_disconnect_handler(nullptr), 
        active_char(nullptr), public_addr(public_addr), connected(false), local_dc(false) {
    gatt.cb_write_response = [this]() {
        active_char->gatt_op_error_handler = nullptr;
        active_char = nullptr;
        write_handler(write_context, active_char, nullptr);
    };
}

WarbleGatt_Blepp::~WarbleGatt_Blepp() {
    gatt.close();

    clear_characteristics();
}

void WarbleGatt_Blepp::clear_characteristics() {
    for(auto it: characteristics) {
        delete it.second;
    }
    characteristics.clear();
}

#include <iostream>
void WarbleGatt_Blepp::connect_async(void* context, FnVoid_VoidP_WarbleGattP_CharP handler) {
    thread th([this, context, handler]() {
        enum State {
            DONE,
            CONNECTING,
            DISCOVER_SERVICE,
            DISCOVER_CHAR
        };
        State connState = CONNECTING;

        bool terminate = false;
        int dc_code;
        local_dc = false;

        gatt.cb_connected = [this, &connState]() {
            cout << "Connected" << endl;
            connected = true;
            connState = DISCOVER_SERVICE;
            gatt.read_primary_services();
        };
        gatt.cb_services_read = [this, &connState]() {
            cout << "Services disconvered" << endl;
            connState = DISCOVER_CHAR;
            gatt.find_all_characteristics();
        };
        gatt.cb_find_characteristics = [this, &connState]() {
            cout << "Characteristics discovered" << endl;
            gatt.get_client_characteristic_configuration();
        };
        gatt.cb_disconnected = [this, &terminate, &dc_code](BLEGATTStateMachine::Disconnect d) {
            if (active_char != nullptr && active_char->gatt_op_error_handler != nullptr) {
                active_char->gatt_op_error_handler(BLEGATTStateMachine::get_disconnect_string(d));
            }

            dc_code = d.error_code;
            terminate = true;
        };
        gatt.cb_get_client_characteristic_configuration = [this, context, handler, &connState]() {
            cout << "Descriptors found" << endl;
            services.clear();
            clear_characteristics();

            for(auto& service: gatt.primary_services) {
                services.insert(uuid_to_string(service.uuid));
    	        for(auto& characteristic: service.characteristics) {
                    characteristics.emplace(uuid_to_string(characteristic.uuid), new WarbleGattChar_Blepp(this, characteristic));
                }
            }

            if (connState != DONE) {
                connState = DONE;
                handler(context, this, nullptr);
            }
        };

        auto socket_select = [this, &terminate](timeval* timeout) {
            fd_set write_set, read_set;
            FD_ZERO(&read_set);
            FD_ZERO(&write_set);

            FD_SET(gatt.socket(), &read_set);
            if(gatt.wait_on_write()) {
                FD_SET(gatt.socket(), &write_set);
            }

            int status = select(gatt.socket() + 1, &read_set, &write_set, nullptr, timeout);
            if (!local_dc && !terminate && status > 0) {
                if(FD_ISSET(gatt.socket(), &write_set)) {
                    gatt.write_and_process_next();
                }

                if(FD_ISSET(gatt.socket(), &read_set)) {
                    gatt.read_and_process_next();
                }
            }

            return status;
        };

        gatt.connect(mac, false, public_addr, hci_mac);
        timeval timeout = { 10, 0 };
        int status = socket_select(&timeout);
        if (status <= 0) {
            gatt.close();
            handler(context, this, status == 0 ? WARBLE_CONNECT_TIMEOUT : WARBLE_GATT_ERROR);
        } else {
            while(!local_dc && !terminate && (status = socket_select(nullptr)) > 0) {
            }

            if (connState != DONE) {
                stringstream buffer;
                if (connState == DISCOVER_SERVICE) {
                    buffer << WARBLE_DISCOVER_SERVICES_ERROR;
                } else if (connState == DISCOVER_CHAR) {
                    buffer << WARBLE_DISCOVER_CHARACTERISTICS_ERROR;
                }
                buffer << " (status = " << status << ")";

                connState = DONE;
                handler(context, this, buffer.str().c_str());
            }
            if (!local_dc) {
                gatt.cb_disconnected = [](BLEGATTStateMachine::Disconnect d) { };
            }
            gatt.close();

            connected = false;
            if (on_disconnect_handler != nullptr) {
                on_disconnect_handler(on_disconnect_context, this, dc_code);
            }
        }
    });
    th.detach();
    swap(blepp_state_machine, th);
}

void WarbleGatt_Blepp::disconnect() {
    local_dc = true;
    shutdown(gatt.socket(), SHUT_RDWR);
}

void WarbleGatt_Blepp::on_disconnect(void* context, FnVoid_VoidP_WarbleGattP_Int handler) {
    on_disconnect_context = context;
    on_disconnect_handler = handler;
}

bool WarbleGatt_Blepp::is_connected() const {
    return connected;
}

WarbleGattChar* WarbleGatt_Blepp::find_characteristic(const std::string& uuid) const {
    auto it = characteristics.find(uuid);
    return it == characteristics.end() ? nullptr : it->second;
}

bool WarbleGatt_Blepp::service_exists(const std::string& uuid) const {
    return services.count(uuid);
}

WarbleGattChar_Blepp::WarbleGattChar_Blepp(WarbleGatt_Blepp* owner, BLEPP::Characteristic& ble_char) : owner(owner), ble_char(ble_char) {
    ble_char.cb_read = [this](const PDUReadResponse& r) {
        this->owner->active_char->gatt_op_error_handler = nullptr;
        this->owner->active_char = nullptr;
        read_handler(read_context, this, r.value().first, r.value().second - r.value().first, nullptr);
    };
    ble_char.cb_notify_or_indicate = [this](const PDUNotificationOrIndication& n) {
        value_changed_handler(value_changed_context, this, n.value().first, n.value().second - n.value().first);
    };

    string str = uuid_to_string(ble_char.uuid);
    strcpy(uuid_str, str.c_str());
}

WarbleGattChar_Blepp::~WarbleGattChar_Blepp() {

}

void WarbleGattChar_Blepp::write_async(const uint8_t* value, uint8_t len, void* context, FnVoid_VoidP_WarbleGattCharP_CharP handler) {
    try {
        gatt_op_error_handler = [this, context, handler](const char* msg) {
            owner->active_char = nullptr;

            if (msg != nullptr) {
                stringstream error_stream;
                string full_msg;

                error_stream << WARBLE_GATT_WRITE_ERROR << "(" << msg << ")";
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

void WarbleGattChar_Blepp::write_without_resp_async(const uint8_t* value, uint8_t len, void* context, FnVoid_VoidP_WarbleGattCharP_CharP handler) {
    stringstream error_stream;
    string msg;
    const char* error_msg = nullptr;

    try {
        ble_char.write_command(value, len);
    } catch (const std::exception& e) {
        error_stream << WARBLE_GATT_WRITE_ERROR << "(" << e.what() << ")";
        msg = error_stream.str();
        error_msg = msg.c_str();
    } catch (const BLEDevice::WriteError&) {
        error_msg = WARBLE_GATT_WRITE_ERROR;
    }

    handler(context, this, error_msg);
}

void WarbleGattChar_Blepp::read_async(void* context, FnVoid_VoidP_WarbleGattCharP_UbyteP_Ubyte_CharP handler) {
    try {
        gatt_op_error_handler = [this, context, handler](const char* msg) {
            owner->active_char = nullptr;

            if (msg != nullptr) {
                stringstream error_stream;
                string full_msg;

                error_stream << WARBLE_GATT_READ_ERROR << "(" << msg << ")";
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

void WarbleGattChar_Blepp::enable_notifications_async(void* context, FnVoid_VoidP_WarbleGattCharP_CharP handler) {
    try {
        gatt_op_error_handler = [this, context, handler](const char* msg) {
            owner->active_char = nullptr;

            if (msg != nullptr) {
                stringstream error_stream;
                string full_msg;

                error_stream << WARBLE_GATT_ENABLE_NOTIFY_ERROR << "(" << msg << ")";
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

void WarbleGattChar_Blepp::disable_notifications_async(void* context, FnVoid_VoidP_WarbleGattCharP_CharP handler) {
    try {
        gatt_op_error_handler = [this, context, handler](const char* msg) {
            owner->active_char = nullptr;

            if (msg != nullptr) {
                stringstream error_stream;
                string full_msg;

                error_stream << WARBLE_GATT_DISABLE_NOTIFY_ERROR << "(" << msg << ")";
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

void WarbleGattChar_Blepp::on_notification_received(void* context, FnVoid_VoidP_WarbleGattCharP_UbyteP_Ubyte handler) {
    value_changed_context = context;
    value_changed_handler = handler;
}

const char* WarbleGattChar_Blepp::get_uuid() const {
    return uuid_str;
}

WarbleGatt* WarbleGattChar_Blepp::get_gatt() const {
    return owner;
}

#endif
