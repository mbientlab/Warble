#include "api_blepp.h"

#include "blepp/blestatemachine.h"
#include "blepp/pretty_printers.h"

#include <fcntl.h>
#include <sys/time.h>
#include <thread>
#include <unistd.h>

using std::function;
using std::swap;
using std::thread;
using std::vector;
using namespace BLEPP;

static timeval connect_timeout = { 10, 0 };

BleatGatt_Blepp::BleatGatt_Blepp(const char* mac) : mac(mac), state_machine_set(false) {
    log_level = Error;
        
    gatt.cb_connected = [this]() {
        gatt.read_primary_services();
    };
    gatt.cb_disconnected = [this](BLEGATTStateMachine::Disconnect d) {
        on_disconnect_handler(on_disconnect_context, this);
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

        connect_handler(connect_context, this);
    };
    gatt.cb_write_response = [this]() {
        write_handler(write_context, active_char);
    };
}

BleatGatt_Blepp::~BleatGatt_Blepp() {
    gatt.close();

    for(auto it: characteristics) {
        delete it.second;
    }
}

void BleatGatt_Blepp::connect_async(void* context, Void_VoidP_BleatGattP handler) {
    connect_context = context;
    connect_handler = handler;

    if (!state_machine_set) {
        thread th([this]() {
            gatt.connect(mac, false, false);

            fd_set write_set, read_set;
            timeval* timeout = &connect_timeout;

            while(true) {
                FD_ZERO(&read_set);
                FD_ZERO(&write_set);

                FD_SET(gatt.socket(), &read_set);
                if(gatt.wait_on_write()) {
                    FD_SET(gatt.socket(), &write_set);
                }

                if (!select(gatt.socket() + 1, &read_set, &write_set, nullptr, timeout)) {
                    state_machine_set = false;
                    gatt.close();
                    connect_handler(connect_context, this);
                    break;
                }

                if(FD_ISSET(gatt.socket(), &write_set)) {
                    gatt.write_and_process_next();
                }

                if(FD_ISSET(gatt.socket(), &read_set)) {
                    gatt.read_and_process_next();
                }

                timeout = nullptr;
                state_machine_set = true;
            }
        });
        swap(blepp_state_machine, th);
    }
}

void BleatGatt_Blepp::disconnect() {
    gatt.close();
}

void BleatGatt_Blepp::on_disconnect(void* context, Void_VoidP_BleatGattP handler) {
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

void BleatGattChar_Blepp::write_async(const uint8_t* value, uint8_t len, void* context, Void_VoidP_BleatGattCharP handler) {
    owner->active_char = this;
    owner->write_context = context;
    owner->write_handler = handler;
    ble_char.write_request(value, len);
}

void BleatGattChar_Blepp::write_without_resp_async(const uint8_t* value, uint8_t len, void* context, Void_VoidP_BleatGattCharP handler) {
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