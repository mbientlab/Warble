#include "win10_scanner.h"

#ifdef API_WIN10

#include <collection.h>
#include <cstdio>
#include <wrl/wrappers/corewrappers.h>

using namespace std;;
using namespace Windows::Devices::Bluetooth::Advertisement;
using namespace Windows::Foundation;
using namespace Windows::Security::Cryptography;
using namespace Platform;

BleatScanner* bleatscanner_create(std::int32_t nopts, const BleatOption* opts) {
    return new BleatScanner_Win10();
}

BleatScanner_Win10::BleatScanner_Win10() : scan_result_context(nullptr), scan_result_handler(nullptr) {
    watcher = ref new BluetoothLEAdvertisementWatcher();
    watcher->ScanningMode = BluetoothLEScanningMode::Active;
    watcher->Received += ref new TypedEventHandler<BluetoothLEAdvertisementWatcher^, BluetoothLEAdvertisementReceivedEventArgs^>([this](BluetoothLEAdvertisementWatcher^ watcher, BluetoothLEAdvertisementReceivedEventArgs^ args) {
        auto it = seen_devices.find(args->BluetoothAddress);
        if (it == seen_devices.end()) {
            if (args->AdvertisementType != BluetoothLEAdvertisementType::ScanResponse) {
                wstring ws_std(args->Advertisement->LocalName->Data());
                seen_devices.emplace(piecewise_construct, forward_as_tuple(args->BluetoothAddress), forward_as_tuple(ws_std.begin(), ws_std.end()));
            }
        } else {
            if (args->AdvertisementType == BluetoothLEAdvertisementType::ScanResponse && scan_result_handler != nullptr) {
                BleatScanMftData *manufacturer_data;
                int i = 0;

                if (args->Advertisement->ManufacturerData->Size) {
                    manufacturer_data = new BleatScanMftData[args->Advertisement->ManufacturerData->Size];

                    for (auto it : args->Advertisement->ManufacturerData) {
                        Array<byte>^ wrapper = ref new Array<byte>(it->Data->Length);
                        CryptographicBuffer::CopyToByteArray(it->Data, &wrapper);

                        manufacturer_data[i].value = wrapper->Data;
                        manufacturer_data[i].value_size = wrapper->Length;
                        manufacturer_data[i].company_id = it->CompanyId;
                        i++;
                    }
                } else {
                    manufacturer_data = nullptr;
                }

                uint64_t mac_raw = args->BluetoothAddress;
                unsigned char* bytes = (unsigned char*) &mac_raw;
                char mac_str[18];
                sprintf_s(mac_str, sizeof(mac_str), "%02x:%02x:%02x:%02x:%02x:%02x", bytes[5], bytes[4], bytes[3], bytes[2], bytes[1], bytes[0]);

                BleatScanResult result = {
                    mac_str,
                    it->second.c_str(),
                    manufacturer_data,
                    i,
                    (int32_t)args->RawSignalStrengthInDBm
                };
                scan_result_handler(scan_result_context, &result);

                delete[] manufacturer_data;
            }
        }
    });
}

BleatScanner_Win10::~BleatScanner_Win10() {
    watcher = nullptr;
}

void BleatScanner_Win10::set_handler(void* context, Void_VoidP_BleatScanResultP handler) {
    scan_result_context = context;
    scan_result_handler = handler;
}

void BleatScanner_Win10::start() {
    watcher->Start();
}

void BleatScanner_Win10::stop() {
    watcher->Stop();
}

#endif
