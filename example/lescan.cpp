#include "bleat/scanner.h"
#include "bleat/lib.h"

#include <csignal>
#include <condition_variable> // std::condition_variable
#include <iostream>
#include <mutex>              // std::mutex, std::unique_lock

using namespace std;

static condition_variable cv;
static void signal_handler(int signum) {
    if (signum == SIGINT) {
        cv.notify_all();
    }
}

int main(int argc, char** argv) {
    cout << "bleat v" << bleat_lib_version() << endl;
    cout << "bleat config: " << bleat_lib_config() << endl;

    mutex m;
    unique_lock<std::mutex> lock(m);
    signal(SIGINT, signal_handler);

    bleat_scanner_set_handler(nullptr, [](void* context, const BleatScanResult* result) {
        cout << "mac: " << result->mac << endl;
        cout << "name: " << result->name << endl;
        cout << "rssi: " << result->rssi << " dBm" << endl;

        for (int i = 0; i < result->manufacturer_data_size; i++) {
            cout << hex << "    company id: 0x" << result->manufacturer_data[i].company_id << endl;
            cout << "    value: [";

            for (int j = 0; j < result->manufacturer_data[i].value_size; j++) {
                if (j != 0) {
                    cout << ", ";
                }
                cout << "0x" << (int)result->manufacturer_data[i].value[j];
            }
            cout << "]" << dec << endl;
        }

        cout << "------" << endl;
    });
    
    bleat_scanner_start(0, nullptr);
    cv.wait(lock);

    bleat_scanner_stop();
    return 0;
}