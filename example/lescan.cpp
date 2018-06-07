#include "bleat/scanner.h"
#include "bleat/lib.h"

#include <chrono>
#include <iostream>
#include <thread>

using namespace std;

int main(int argc, char** argv) {
    cout << "bleat v" << bleat_lib_version() << endl;
    cout << "bleat config: " << bleat_lib_config() << endl;

    bleat_scanner_set_handler(nullptr, [](void* context, const BleatScanResult* result) {
        cout << "mac: " << result->mac << endl;
        cout << "name: " << result->name << endl;
        cout << "rssi: " << result->rssi << " dBm" << endl;

        cout << "metawear service? " << bleat_scan_result_has_service_uuid(result, "326a9000-85cb-9195-d9dd-464cfbbae75a") << endl;
        cout << "mbientlab manufacturer data? ";

        const BleatScanMftData* man_data;
        if ((man_data = bleat_scan_result_get_manufacturer_data(result, 0x626d)) != nullptr) {
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
    
    bleat_scanner_start(0, nullptr);
    this_thread::sleep_for(10s);

    bleat_scanner_stop();
    return 0;
}