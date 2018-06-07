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

    auto gatt = bleat_gatt_create(argv[1]);
    bleat_gatt_connect_async(gatt, nullptr, [](void* context, BleatGatt* caller, const char* value) {
        if (value != nullptr) {
            cout << "Error connecting: " << value << endl;
        } else {
            cout << "Connected" << endl;
        }
        this_thread::sleep_for(5s);
        cv.notify_all();
    });
    cv.wait(lock);

    bleat_gatt_disconnect(gatt);
    
    cout << "disconnected" << endl;
    return 0;
}