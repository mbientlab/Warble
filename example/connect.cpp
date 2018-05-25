#include "bleat/bleat.h"

#include <chrono>
#include <functional>
#include <iostream>
#include <iomanip>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

#include <csignal>
#include <condition_variable> // std::condition_variable
#include <mutex>              // std::mutex, std::unique_lock

using namespace std;
using namespace std::chrono;
using namespace std::chrono_literals;

static condition_variable cv;

static void signal_handler(int signum) {
    if (signum == SIGABRT) {
        cv.notify_all();
    }
}

static void bytes_to_hex_string(ostream& os, uint8_t length, const uint8_t* value) {
    os << "[" << hex << setw(2) << setfill('0');
    for (uint8_t i = 0; i < length; i++) {
        os << (i == 0 ? "0x" : ", 0x") << (int)value[i];
    }
    os << "]" << dec;
}

static uint32_t samples = 0;
static queue<vector<uint8_t>> values;
static function<void(BleatGattChar*)> write_values_handler;
static void write_values(BleatGattChar* gatt_char) {
    if (!values.empty()) {
        bleat_gattchar_write_async(gatt_char, values.front().data(), values.front().size(), nullptr, [](void* context, BleatGattChar* caller, const char* value) {
            values.pop();
            write_values(caller);
        });
    } else {
        write_values_handler(gatt_char);
    }
}

static function<void(BleatGatt*)> reconnect_handler;
static void reconnect_async(BleatGatt* gatt) {
    cerr << "Reconnecting..." << endl;
    bleat_gatt_connect_async(gatt, nullptr, [](void* context, BleatGatt* gatt, const char* value) {
        if (value == nullptr) {
            reconnect_handler(gatt);
        } else {
            reconnect_async(gatt);
        }
    });
}

time_point<system_clock> start;

int main(int argc, char** argv) {
    mutex m;
    unique_lock<std::mutex> lock(m);

    signal(SIGINT, signal_handler);

    auto cgatt = bleat_gatt_new(argv[1]);
    bleat_gatt_on_disconnect(cgatt, nullptr, [](void* context, BleatGatt* caller, uint32_t status) {
        cout << "i am disconnected: " << status << endl;
        reconnect_handler = [](BleatGatt* gatt) {
            cout << "connection established" << endl;
        };
        reconnect_async(caller);
    });
    bleat_gatt_connect_async(cgatt, nullptr, [](void* context, BleatGatt* caller, const char* value) {
        if (value != nullptr) {
            cout << "Error connecting: " << value << endl;
            return;
        }
        auto cmd = bleat_gatt_find_characteristic(caller, "326a9001-85cb-9195-d9dd-464cfbbae75a");

        uint8_t conn_param[] = { 0x11, 0x09, 0x06, 0x00, 0x06, 0x00, 0x00, 0x00, 0x58, 0x02 };
        bleat_gattchar_write_async(cmd, conn_param, sizeof(conn_param), caller, [](void* context, BleatGattChar* caller, const char* value) {
            cout << "you may proceed" << endl;

            auto notify = bleat_gatt_find_characteristic((BleatGatt*)context, "326a9006-85cb-9195-d9dd-464cfbbae75a");
            bleat_gattchar_set_value_changed_handler(notify, nullptr, [](void* context, BleatGattChar* caller, const uint8_t* value, uint8_t length) {
                samples++;
                cout << "samples = " << samples << endl;
            });
            bleat_gattchar_enable_notifications_async(notify, caller, [](void* context, BleatGattChar* caller, const char* value) {
                /*
                values.push({0x03, 0x03, 0x29, 0x0c});
                values.push({0x03, 0x02, 0x01, 0x00});
                values.push({0x03, 0x04, 0x01});

                values.push({0x13, 0x03, 0x29, 0x00});
                values.push({0x13, 0x05, 0x01});
                values.push({0x13, 0x02, 0x01, 0x00});

                values.push({0x03, 0x01, 0x01});
                values.push({0x13, 0x01, 0x01});
                */

                values.push({ 0x1, 0x1, 0x1 });
                write_values_handler = [](BleatGattChar* caller) {
                    start = system_clock::now();
                    cout << "ready" << endl;
                };
                write_values((BleatGattChar*)context);
            });
        });
        this_thread::sleep_for(2s);
    });

    cv.wait(lock);

    values.push({ 0xfe, 0x1 });
    write_values_handler = [](BleatGattChar* caller) {
        cv.notify_all();
    };
    write_values(bleat_gatt_find_characteristic(cgatt, "326a9001-85cb-9195-d9dd-464cfbbae75a"));
    auto end = system_clock::now();
    cv.wait(lock);

    cout << samples << endl;
    duration<double> diff = end - start;
    cout << diff.count() << endl;

    bleat_gatt_disconnect(cgatt);
    return 0;
}