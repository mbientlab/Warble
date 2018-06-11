#include "warble/scanner.h"
#include "warble/lib.h"

#include <chrono>
#include <iostream>
#include <thread>

using namespace std;

int main(int argc, char** argv) {
    cout << "warble v" << warble_lib_version() << endl;
    cout << "warble config: " << warble_lib_config() << endl;

    warble_scanner_set_handler(nullptr, [](void* context, const WarbleScanResult* result) {
        cout << "mac: " << result->mac << endl;
        cout << "name: " << result->name << endl;
        cout << "rssi: " << result->rssi << " dBm" << endl;

        cout << "metawear service? " << warble_scan_result_has_service_uuid(result, "326a9000-85cb-9195-d9dd-464cfbbae75a") << endl;
        cout << "mbientlab manufacturer data? ";

        const WarbleScanMftData* man_data;
        if ((man_data = warble_scan_result_get_manufacturer_data(result, 0x626d)) != nullptr) {
            cout << endl << "    value: [";

            for (int j = 0; j < man_data->value_size; j++) {
                if (j != 0) {
                    cout << ", ";
                }
                cout << "0x" << (int)man_data->value[j];
            }
            cout << "]" << dec << endl;
        } else {
            cout << "false" << endl;
        }

        cout << "------" << endl;
    });
    
    warble_scanner_start(0, nullptr);
    this_thread::sleep_for(10s);

    warble_scanner_stop();
    return 0;
}