/**
 * @copyright MbientLab License
 */
#ifdef API_WIN10

#include "scanner_def.h"

#include <collection.h>
#include <cstdio>
#include <wrl/wrappers/corewrappers.h>

using namespace std;
using namespace Windows::Devices::Bluetooth::Advertisement;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Security::Cryptography;
using namespace Platform;

class WarbleScanner_Win10 : public WarbleScanner {
public:
    WarbleScanner_Win10();
    virtual ~WarbleScanner_Win10();

    virtual void set_handler(void* context, FnVoid_VoidP_WarbleScanResultP handler);
    virtual void start(int32_t nopts, const WarbleOption* opts);
    virtual void stop();

private:
    void* scan_result_context;
    FnVoid_VoidP_WarbleScanResultP scan_result_handler;

    unordered_map<uint64_t, WarbleScanPrivateData> seen_devices;
    unordered_map<uint64_t, string> device_names;
    BluetoothLEAdvertisementWatcher^ watcher;
};

WarbleScanner* warble_scanner_create() {
    return new WarbleScanner_Win10();
}

WarbleScanner_Win10::WarbleScanner_Win10() : scan_result_context(nullptr), scan_result_handler(nullptr) {
    watcher = ref new BluetoothLEAdvertisementWatcher();
    watcher->ScanningMode = BluetoothLEScanningMode::Active;
    watcher->Received += ref new TypedEventHandler<BluetoothLEAdvertisementWatcher^, BluetoothLEAdvertisementReceivedEventArgs^>([this](BluetoothLEAdvertisementWatcher^ watcher, BluetoothLEAdvertisementReceivedEventArgs^ args) {
        auto it = seen_devices.find(args->BluetoothAddress);

        if (it == seen_devices.end()) {
            WarbleScanPrivateData private_data;
            seen_devices.emplace(args->BluetoothAddress, private_data);
            it = seen_devices.find(args->BluetoothAddress);
        }

        if (args->AdvertisementType != BluetoothLEAdvertisementType::ScanResponse) {
            for (auto iter = args->Advertisement->ServiceUuids->First(); iter->HasCurrent; iter->MoveNext()) {
                wstring wide(iter->Current.ToString()->Data());
                string str = string(wide.begin(), wide.end()).substr(1, 36);
                it->second.service_uuids.emplace(str);
            }
            {
                wstring wide(args->Advertisement->LocalName->Data());
                device_names[args->BluetoothAddress] = string(wide.begin(), wide.end());
            }
        } else if (scan_result_handler != nullptr) {
            it->second.manufacturer_data.clear();

            Platform::Collections::Vector<Object^> mft_data;
            for (auto data_it : args->Advertisement->ManufacturerData) {
                Array<byte>^ wrapper = ref new Array<byte>(data_it->Data->Length);
                CryptographicBuffer::CopyToByteArray(data_it->Data, &wrapper);

                WarbleScanMftData data = {
                    wrapper->Data,
                    wrapper->Length
                };
                it->second.manufacturer_data.emplace(data_it->CompanyId, data);

                mft_data.Append(wrapper);
            }

            uint64_t mac_raw = args->BluetoothAddress;
            unsigned char* bytes = (unsigned char*)&mac_raw;
            char mac_str[18];
            sprintf_s(mac_str, sizeof(mac_str), "%02X:%02X:%02X:%02X:%02X:%02X", bytes[5], bytes[4], bytes[3], bytes[2], bytes[1], bytes[0]);

            WarbleScanResult result = {
                mac_str,
                device_names[args->BluetoothAddress].c_str(),
                (int32_t)args->RawSignalStrengthInDBm,
                &it->second
            };
            scan_result_handler(scan_result_context, &result);
        }
    });
}

WarbleScanner_Win10::~WarbleScanner_Win10() {
    watcher->Stop();
    watcher = nullptr;
}

void WarbleScanner_Win10::set_handler(void* context, FnVoid_VoidP_WarbleScanResultP handler) {
    scan_result_context = context;
    scan_result_handler = handler;
}

void WarbleScanner_Win10::start(int32_t nopts, const WarbleOption* opts) {
    watcher->Start();
}

void WarbleScanner_Win10::stop() {
    watcher->Stop();
}

#endif
