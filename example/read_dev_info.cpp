// compile: g++ -o read_dev_info example/read_dev_info.cpp -std=c++14 -Isrc -Ldist/release/lib/x64 -Ldeps/libblepp -lbleat -lpthread -lble++
#include "bleat/bleat.h"

#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>

using namespace std;

static function<void(void)> read_completed;
static queue<const char*> uuids;
static void read_char_values(BleatGatt* gatt) {
    if (!uuids.size()) {
        read_completed();
        return;
    }

    auto next = uuids.front();
    auto gatt_char = bleat_gatt_find_characteristic(gatt, next);

    cout << next;
    if (gatt_char == nullptr) {
        cout << ": not found " << endl;
    } else {
        bleat_gattchar_read_async(gatt_char, gatt, [](void* context, BleatGattChar* gatt_char, const uint8_t* value, uint8_t length, const char* error) {
            if (error != nullptr) {
                cout << ": error reading value (" << error << ")" << endl;
            } else {
                cout << ": " << string(value, value + length) << endl;
            }

            BleatGatt* gatt = (BleatGatt*) context;
            uuids.pop();

            read_char_values(gatt);
        });
    }
}

static condition_variable cv;

int main(int argc, char** argv) {
    mutex m;
    unique_lock<std::mutex> lock(m);

    auto gatt = bleat_gatt_create(argv[1]);

    bleat_gatt_connect_async(gatt, nullptr, [](void* context, BleatGatt* caller, const char* error) {
        uuids.push("00002a26-0000-1000-8000-00805f9b34fb");
        uuids.push("00002a24-0000-1000-8000-00805f9b34fb");
        uuids.push("00002a27-0000-1000-8000-00805f9b34fb");
        uuids.push("00002a29-0000-1000-8000-00805f9b34fb");
        uuids.push("00002a25-0000-1000-8000-00805f9b34fb");

        read_completed = []() {
            cv.notify_all();
        };

        read_char_values(caller);
    });

    cv.wait(lock);

    return 0;
}