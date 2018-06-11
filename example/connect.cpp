// compile: g++ -o connect example/connect.cpp -std=c++14 -Isrc -Ldist/release/lib/x64 -Ldeps/libblepp -lwarble -lpthread -lble++
#include "warble/warble.h"

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

int main(int argc, char** argv) {
    if (argc < 2) {
        cerr << "usage: connect [device mac] [hci mac](optional)" << endl;
        return 1;
    }

    mutex m;
    unique_lock<std::mutex> lock(m);

#ifdef WIN32
    auto gatt = warble_gatt_create(argv[1]);
#else
    // Setting hci mac only supported on Linux
    WarbleOption config_options[2] = {
        {"mac", argv[1]}
    };
    if (argc >= 3) {
        config_options[1].key = "hci";
        config_options[1].value = argv[2];
    }

    auto gatt = warble_gatt_create_with_options(argc - 1, config_options);
#endif
    warble_gatt_connect_async(gatt, nullptr, [](void* context, WarbleGatt* caller, const char* value) {
        if (value != nullptr) {
            cout << "Error connecting: " << value << endl;
        }
        cout << "Connected? " << warble_gatt_is_connected(caller) << endl;

        this_thread::sleep_for(5s);
        cv.notify_all();
    });
    cv.wait(lock);

    warble_gatt_disconnect(gatt);
    
    cout << "Donnected? " << warble_gatt_is_connected(gatt) << endl;
    return 0;
}
