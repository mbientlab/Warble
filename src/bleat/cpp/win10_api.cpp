#include "win10_api.h"

#ifdef API_win10

#include <collection.h>
#include <cstring>
#include <functional>
#include <pplawait.h>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_map>
#include <wrl/wrappers/corewrappers.h>

using namespace concurrency;
using namespace std;
using namespace Windows::Devices::Bluetooth;
using namespace Windows::Devices::Bluetooth::Advertisement;
using namespace Windows::Devices::Bluetooth::GenericAttributeProfile;
using namespace Windows::Foundation;
using namespace Windows::Security::Cryptography;
using namespace Platform;

BleatGatt_Win10::BleatGatt_Win10(int nopts, const BleatGattOption* opts) : 
        mac(nullptr), device(nullptr), connect_context(nullptr), on_disconnect_context(nullptr), connect_handler(nullptr), on_disconnect_handler(nullptr) {
    unordered_map<string, function<void(const char*)>> arg_processors = {
        { "mac", [this](const char* value) {mac = value; } }
    };

    for (int i = 0; i < nopts; i++) {
        auto it = arg_processors.find(opts[i].key);
        if (it == arg_processors.end()) {
            throw runtime_error(string("option '") + opts[i].key + "' does not exist");
        }
        (it->second)(opts[i].value);
    }
    if (mac == nullptr) {
        throw runtime_error("option 'mac' was not set");
    }
}

BleatGatt_Win10::~BleatGatt_Win10() {
    cleanup();
}

#include <iostream>

void BleatGatt_Win10::connect_async(void* context, Void_VoidP_BleatGattP_Uint handler) {
    connect_context = context;
    connect_handler = handler;

    task_completion_event<void> discover_device_event;
    if (device != nullptr) {
        discover_device_event.set();
    } else {
        string mac_copy(mac);
        mac_copy.erase(2, 1);
        mac_copy.erase(4, 1);
        mac_copy.erase(6, 1);
        mac_copy.erase(8, 1);
        mac_copy.erase(10, 1);

        size_t temp;
        uint64_t mac_ulong = stoul(mac_copy.c_str(), &temp, 16);
        create_task(BluetoothLEDevice::FromBluetoothAddressAsync(mac_ulong)).then([&discover_device_event, this](BluetoothLEDevice^ device) {
            cookie = device->ConnectionStatusChanged += ref new TypedEventHandler<BluetoothLEDevice^, Object^>([this](BluetoothLEDevice^ sender, Object^ args) {
                switch (sender->ConnectionStatus) {
                case BluetoothConnectionStatus::Disconnected:

                    if (on_disconnect_handler != nullptr) {
                        on_disconnect_handler(on_disconnect_context, this, 0);
                    }
                    break;
                }
            });

            this->device = device;
            discover_device_event.set();
        }).then([&discover_device_event](task<void> previous_task) {
            try {
                previous_task.get();
            } catch (...) {
                discover_device_event.set_exception(runtime_error("Failed to discover device"));
            }
        });
    }

    create_task(discover_device_event).then([this]() {
        return create_task(device->GetGattServicesAsync());
    }).then([](GattDeviceServicesResult^ result) {
        vector<task<GattCharacteristicsResult^>> find_gattchar_tasks;

        if (result->Status == GattCommunicationStatus::Success) {
            for (auto it : result->Services) {
                find_gattchar_tasks.push_back(create_task(it->GetCharacteristicsAsync()));
            }

            return when_all(begin(find_gattchar_tasks), end(find_gattchar_tasks));
        }

        throw runtime_error("Failed to discover gatt services");
    }).then([this](vector<GattCharacteristicsResult^> results) {
        for (auto it : results) {
            if (it->Status == GattCommunicationStatus::Success) {
                for (auto it2 : it->Characteristics) {
                    characteristics.emplace(it2->Uuid, new BleatGattChar_Win10(it2));
                }
            } else {
                throw runtime_error("Failed to discover gatt characteristics");
            }
        }
    }).then([this](task<void> previous) {
        try {
            previous.wait();
            connect_handler(connect_context, this, BLEAT_GATT_STATUS_CONNECT_OK);
        } catch (const exception&) {
            connect_handler(connect_context, this, BLEAT_GATT_STATUS_CONNECT_GATT_ERROR);
        }
    });
}

void BleatGatt_Win10::disconnect() {
    cleanup();

    if (on_disconnect_handler != nullptr) {
        on_disconnect_handler(on_disconnect_context, this, 0);
    }
}

void BleatGatt_Win10::cleanup() {
    for (auto it : characteristics) {
        delete it.second;
    }
    characteristics.clear();

    device->ConnectionStatusChanged -= cookie;
    device = nullptr;
}

void BleatGatt_Win10::on_disconnect(void* context, Void_VoidP_BleatGattP_Uint handler) {
    on_disconnect_context = context;
    on_disconnect_handler = handler;
}

BleatGattChar* BleatGatt_Win10::find_characteristic(const std::string& uuid) {
    wstring wide_uuid(uuid.begin(), uuid.end());
    auto casted = ref new Platform::String(wide_uuid.c_str());
    GUID rawguid;
    HRESULT hr = IIDFromString(casted->Data(), &rawguid);

    if (SUCCEEDED(hr)) {
        auto it = characteristics.find(rawguid);
        return it == characteristics.end() ? nullptr : it->second;
    }
    return nullptr;
}

BleatGattChar_Win10::BleatGattChar_Win10(GattCharacteristic^ characteristic) : characteristic(characteristic) {

}

BleatGattChar_Win10::~BleatGattChar_Win10() {
    characteristic->ValueChanged -= cookie;
    characteristic = nullptr;
}

void BleatGattChar_Win10::write_async(const uint8_t* value, uint8_t len, void* context, Void_VoidP_BleatGattCharP handler) {
    write_inner_async(GattWriteOption::WriteWithResponse, value, len, context, handler);
}

void BleatGattChar_Win10::write_without_resp_async(const uint8_t* value, uint8_t len, void* context, Void_VoidP_BleatGattCharP handler) {
    write_inner_async(GattWriteOption::WriteWithoutResponse, value, len, context, handler);
}

void BleatGattChar_Win10::read_async(void* context, Void_VoidP_BleatGattCharP_UbyteC_Ubyte handler) {
    create_task(characteristic->ReadValueAsync()).then([context, handler, this](GattReadResult^ result) {
        if (result->Status == GattCommunicationStatus::Success) {
            Array<byte>^ wrapper = ref new Array<byte>(result->Value->Length);
            CryptographicBuffer::CopyToByteArray(result->Value, &wrapper);
            handler(context, this, (uint8_t*)wrapper->Data, wrapper->Length);
        }
    });
}

void BleatGattChar_Win10::enable_notifications_async(void* context, Void_VoidP_BleatGattCharP handler) {
    create_task(characteristic->WriteClientCharacteristicConfigurationDescriptorAsync(GattClientCharacteristicConfigurationDescriptorValue::Notify))
        .then([context, handler, this](GattCommunicationStatus status) {
            handler(context, this);
        });
}

void BleatGattChar_Win10::disable_notifications_async(void* context, Void_VoidP_BleatGattCharP handler) {
    create_task(characteristic->WriteClientCharacteristicConfigurationDescriptorAsync(GattClientCharacteristicConfigurationDescriptorValue::None))
        .then([context, handler, this](GattCommunicationStatus status) {
            characteristic->ValueChanged -= cookie;
            handler(context, this);
        });
}

void BleatGattChar_Win10::set_value_changed_handler(void* context, Void_VoidP_BleatGattCharP_UbyteC_Ubyte handler) {
    cookie = characteristic->ValueChanged += ref new TypedEventHandler<GattCharacteristic^, GattValueChangedEventArgs^>([context, handler, this](GattCharacteristic^ sender, GattValueChangedEventArgs^ obj) {
        Array<byte>^ wrapper = ref new Array<byte>(obj->CharacteristicValue->Length);
        CryptographicBuffer::CopyToByteArray(obj->CharacteristicValue, &wrapper);
        handler(context, this, (uint8_t*)wrapper->Data, wrapper->Length);
    });
}

#endif
