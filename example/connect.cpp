// compile: g++ -o connect example/connect.cpp -std=c++14 -Isrc -Ldist/release/lib/x64 -lwarble
#include "warble/warble.h"

#include <chrono>
#include <future>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <utility>

using namespace std;
using namespace std::chrono;
using namespace std::chrono_literals;

int main(int argc, char** argv) {
    if (argc < 2) {
        cerr << "usage: connect [device mac] [hci mac](optional)" << endl;
        return 1;
    }

#ifdef _WINDOWS
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
    for(int i = 0; i < 3; i++) {
        promise<void> connect_task;
        cout << "Connecting to " << argv[1] << endl;
        warble_gatt_connect_async(gatt, &connect_task, [](void* context, WarbleGatt* caller, const char* value) {
            auto task = (promise<void>*) context;

            if (value != nullptr) {
                cout << "You done fucked up" << endl;
                task->set_exception(make_exception_ptr(runtime_error(value)));
            } else {
                cout << "Am I connected? " << warble_gatt_is_connected(caller) << endl;

                this_thread::sleep_for(5s);
                task->set_value();
            }
        });
        connect_task.get_future().get();

        promise<int32_t> dc_task;
        cout << "Disconnecting..." << endl;
        warble_gatt_on_disconnect(gatt, &dc_task, [](void* context, WarbleGatt* caller, int32_t status) {
            ((promise<int32_t>*) context)->set_value(status);
        });
        warble_gatt_disconnect(gatt);
        cout << "disconnected, status = " << dc_task.get_future().get() << endl;
        
        cout << "Am I connected? " << warble_gatt_is_connected(gatt) << endl;
    }
    return 0;
}
