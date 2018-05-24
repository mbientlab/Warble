/**
 * @copyright MbientLab License
 */
#include "api_blepp.h"

#include "blepp/blestatemachine.h"
#include "blepp/pretty_printers.h"

#include <condition_variable>
#include <fcntl.h>
#include <stdexcept>
#include <sys/time.h>
#include <thread>
#include <unistd.h>
#include <unordered_map>

using namespace std;
using namespace BLEPP;

BleatGatt_Blepp::BleatGatt_Blepp(int nopts, const BleatGattOption* opts) : mac(nullptr), device(nullptr), cv_lock(cv_m), terminate_state_machine(true) {
    unordered_map<string, function<void(const char*)>> arg_processors = {
        {"mac", [this](const char* value) { mac = value; }}, 
        {"hci", [this](const char* value) { device = value; }},
        {"log-level", [this](const char* value) {
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

    gatt.cb_connected = [this]() {
        gatt.read_primary_services();
    };
    gatt.cb_disconnected = [this](BLEGATTStateMachine::Disconnect d) {
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

        connect_handler(connect_context, this, BLEAT_GATT_STATUS_CONNECT_OK);
    };
    gatt.cb_write_response = [this]() {
        write_handler(write_context, active_char);
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
                connect_handler(connect_context, this, status == 0 ? BLEAT_GATT_STATUS_CONNECT_TIMEOUT : BLEAT_GATT_STATUS_CONNECT_GATT_ERROR);
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

void BleatGatt_Blepp::connect_async(void* context, Void_VoidP_BleatGattP_Uint handler) {
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
        read_handler(read_context, this, r.value().first, r.value().second - r.value().first);
    };
    ble_char.cb_notify_or_indicate = [this](const PDUNotificationOrIndication& n) {
        value_changed_handler(value_changed_context, this, n.value().first, n.value().second - n.value().first);
    };
}

BleatGattChar_Blepp::~BleatGattChar_Blepp() {

}

void BleatGattChar_Blepp::write_async(const UBYTE* value, UBYTE len, void* context, Void_VoidP_BleatGattCharP handler) {
    owner->active_char = this;
    owner->write_context = context;
    owner->write_handler = handler;
    ble_char.write_request(value, len);
}

void BleatGattChar_Blepp::write_without_resp_async(const UBYTE* value, UBYTE len, void* context, Void_VoidP_BleatGattCharP handler) {
    ble_char.write_command(value, len);
    handler(context, this);
}

void BleatGattChar_Blepp::read_async(void* context, Void_VoidP_BleatGattCharP_UbyteC_Ubyte handler) {
    read_context = context;
    read_handler = handler;
    ble_char.read_request();
}

void BleatGattChar_Blepp::enable_notifications_async(void* context, Void_VoidP_BleatGattCharP handler) {
    owner->active_char = this;
    owner->write_context = context;
    owner->write_handler = handler;
    ble_char.set_notify_and_indicate(true, false);
}

void BleatGattChar_Blepp::disable_notifications_async(void* context, Void_VoidP_BleatGattCharP handler) {
    owner->active_char = this;
    owner->write_context = context;
    owner->write_handler = handler;
    ble_char.set_notify_and_indicate(false, false);
}

void BleatGattChar_Blepp::set_value_changed_handler(void* context, Void_VoidP_BleatGattCharP_UbyteC_Ubyte handler) {
    value_changed_context = context;
    value_changed_handler = handler;
}