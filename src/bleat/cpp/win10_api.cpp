#ifdef API_WIN10

#include "error_messages.h"
#include "gatt_def.h"
#include "gattchar_def.h"

#include <collection.h>
#include <cstring>
#include <functional>
#include <pplawait.h>
#include <ppltasks.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <wrl/wrappers/corewrappers.h>
#include <Windows.Devices.Bluetooth.h>

using namespace concurrency;
using namespace std;
using namespace Windows::Devices::Bluetooth;
using namespace Windows::Devices::Bluetooth::Advertisement;
using namespace Windows::Devices::Bluetooth::GenericAttributeProfile;
using namespace Windows::Foundation;
using namespace Windows::Security::Cryptography;
using namespace Platform;

struct BleatGattChar_Win10 : public BleatGattChar {
    BleatGattChar_Win10(Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristic^ characteristic);

    virtual ~BleatGattChar_Win10();

    virtual void write_async(const uint8_t* value, uint8_t len, void* context, Void_VoidP_BleatGattCharP_CharP handler);
    virtual void write_without_resp_async(const uint8_t* value, uint8_t len, void* context, Void_VoidP_BleatGattCharP_CharP handler);

    virtual void read_async(void* context, Void_VoidP_BleatGattCharP_UbyteP_Ubyte_CharP handler);

    virtual void enable_notifications_async(void* context, Void_VoidP_BleatGattCharP_CharP handler);
    virtual void disable_notifications_async(void* context, Void_VoidP_BleatGattCharP_CharP handler);
    virtual void set_value_changed_handler(void* context, Void_VoidP_BleatGattCharP_UbyteP_Ubyte handler);

private:
    inline void write_inner_async(GattWriteOption option, const uint8_t* value, uint8_t len, void* context, Void_VoidP_BleatGattCharP_CharP handler) {
        Array<byte>^ wrapper = ref new Array<byte>(len);
        for (uint8_t i = 0; i < len; i++) {
            wrapper[i] = value[i];
        }

        create_task(characteristic->WriteValueAsync(CryptographicBuffer::CreateFromByteArray(wrapper), option))
            .then([context, handler, this](GattCommunicationStatus status) {
            if (status != GattCommunicationStatus::Success) {
                handler(context, this, BLEAT_GATT_WRITE_ERROR);
            } else {
                handler(context, this, nullptr);
            }
        });
    }

    GattCharacteristic^ characteristic;
    Windows::Foundation::EventRegistrationToken cookie;
};

struct Hasher {
    size_t operator() (Guid key) const {
        return key.GetHashCode();
    }
};
struct EqualFn {
    bool operator() (Guid t1, Guid t2) const {
        return t1.Equals(t2);
    }
};


struct BleatGatt_Win10 : public BleatGatt {
    BleatGatt_Win10(const char* mac, BluetoothAddressType addr_type);
    virtual ~BleatGatt_Win10();

    virtual void connect_async(void* context, Void_VoidP_BleatGattP_CharP handler);
    virtual void disconnect();
    virtual void on_disconnect(void* context, Void_VoidP_BleatGattP_Int handler);

    virtual BleatGattChar* find_characteristic(const string& uuid);
    virtual bool service_exists(const string& uuid);

private:
    void cleanup();

    string mac;

    void *on_disconnect_context;
    Void_VoidP_BleatGattP_Int on_disconnect_handler;

    concurrency::task<void> discover_task, connect_task;

    BluetoothLEDevice^ device;
    BluetoothAddressType addr_type;
    Windows::Foundation::EventRegistrationToken cookie;
    unordered_map<Guid, BleatGattChar_Win10*, Hasher, EqualFn> characteristics;
    unordered_set<Guid, Hasher, EqualFn> services;
};

BleatGatt* bleatgatt_create(int32_t nopts, const BleatOption* opts) {
    const char* mac = nullptr;
    BluetoothAddressType addr_type = BluetoothAddressType::Random;
    unordered_map<string, function<void(const char*)>> arg_processors = {
        { "mac", [&mac](const char* value) {mac = value; } },
        { "address-type", [&addr_type](const char* value) {
            if (!strcmp(value, "public")) {
                addr_type = BluetoothAddressType::Public;
            } else if (!strcmp(value, "unspecified")) {
                addr_type = BluetoothAddressType::Unspecified;
            } else if (strcmp(value, "random")) {
                throw runtime_error("invalid value for \'address-type\' option (win10 api): one of [public, random, unspecified]");
            }
        }}
    };

    for (int i = 0; i < nopts; i++) {
        auto it = arg_processors.find(opts[i].key);
        if (it == arg_processors.end()) {
            throw runtime_error(string("option '") + opts[i].key + "' does not exist");
        }
        (it->second)(opts[i].value);
    }
    if (mac == nullptr) {
        throw runtime_error("required option 'mac' was not set");
    }

    return new BleatGatt_Win10(mac, addr_type);
}

BleatGatt_Win10::BleatGatt_Win10(const char* mac, BluetoothAddressType addr_type) : mac(mac), device(nullptr), on_disconnect_context(nullptr), on_disconnect_handler(nullptr), addr_type(addr_type) {
}

BleatGatt_Win10::~BleatGatt_Win10() {
    cleanup();
}

void BleatGatt_Win10::connect_async(void* context, Void_VoidP_BleatGattP_CharP handler) {
    task_completion_event<void> discover_device_event;
    task<void> event_set(discover_device_event);

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
        uint64_t mac_ulong = stoull(mac_copy.c_str(), &temp, 16);
        discover_task = create_task(BluetoothLEDevice::FromBluetoothAddressAsync(mac_ulong, addr_type)).then([discover_device_event, this](BluetoothLEDevice^ device) {
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
        }).then([discover_device_event](task<void> previous_task) {
            try {
                previous_task.get();
            } catch (...) {
                discover_device_event.set_exception(runtime_error("Failed to discover device"));
            }
        });
    }

    connect_task = event_set.then([this]() {
        return create_task(device->GetGattServicesAsync());
    }).then([this](GattDeviceServicesResult^ result) {
        vector<task<GattCharacteristicsResult^>> find_gattchar_tasks;
        if (result->Status == GattCommunicationStatus::Success) {
            for (auto it : result->Services) {
                services.insert(it->Uuid);
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
    }).then([this, handler, context](task<void> previous) {
        try {
            previous.wait();
            handler(context, this, nullptr);
        } catch (const exception& e) {
            handler(context, this, e.what());
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

void BleatGatt_Win10::on_disconnect(void* context, Void_VoidP_BleatGattP_Int handler) {
    on_disconnect_context = context;
    on_disconnect_handler = handler;
}

#define UUID_TO_GUID(uuid)\
    wstring wide_uuid(uuid.begin(), uuid.end());\
    wstringstream stream;\
    stream << L'{' << wide_uuid << L'}';\
\
    auto casted = ref new Platform::String(stream.str().c_str());\
    GUID rawguid;\
    HRESULT hr = IIDFromString(casted->Data(), &rawguid);

BleatGattChar* BleatGatt_Win10::find_characteristic(const string& uuid) {
    UUID_TO_GUID(uuid);

    if (SUCCEEDED(hr)) {
        auto it = characteristics.find(rawguid);
        return it == characteristics.end() ? nullptr : it->second;
    }
    return nullptr;
}

bool BleatGatt_Win10::service_exists(const string& uuid) {
    UUID_TO_GUID(uuid);    

    if (SUCCEEDED(hr)) {
        return services.count(rawguid);
    }
    return 0;
}

BleatGattChar_Win10::BleatGattChar_Win10(GattCharacteristic^ characteristic) : characteristic(characteristic) {

}

BleatGattChar_Win10::~BleatGattChar_Win10() {
    characteristic->ValueChanged -= cookie;
    characteristic = nullptr;
}

void BleatGattChar_Win10::write_async(const uint8_t* value, uint8_t len, void* context, Void_VoidP_BleatGattCharP_CharP handler) {
    write_inner_async(GattWriteOption::WriteWithResponse, value, len, context, handler);
}

void BleatGattChar_Win10::write_without_resp_async(const uint8_t* value, uint8_t len, void* context, Void_VoidP_BleatGattCharP_CharP handler) {
    write_inner_async(GattWriteOption::WriteWithoutResponse, value, len, context, handler);
}

void BleatGattChar_Win10::read_async(void* context, Void_VoidP_BleatGattCharP_UbyteP_Ubyte_CharP handler) {
    create_task(characteristic->ReadValueAsync()).then([context, handler, this](GattReadResult^ result) {
        if (result->Status == GattCommunicationStatus::Success) {
            Array<byte>^ wrapper = ref new Array<byte>(result->Value->Length);
            CryptographicBuffer::CopyToByteArray(result->Value, &wrapper);
            handler(context, this, (uint8_t*)wrapper->Data, wrapper->Length, nullptr);
        } else {
            handler(context, this, nullptr, 0, BLEAT_GATT_READ_ERROR);
        }
    });
}

void BleatGattChar_Win10::enable_notifications_async(void* context, Void_VoidP_BleatGattCharP_CharP handler) {
    create_task(characteristic->WriteClientCharacteristicConfigurationDescriptorAsync(GattClientCharacteristicConfigurationDescriptorValue::Notify))
        .then([context, handler, this](GattCommunicationStatus status) {
            if (status == GattCommunicationStatus::Success) {
                handler(context, this, nullptr);
            } else {
                handler(context, this, BLEAT_GATT_ENABLE_NOTIFY_ERROR);
            }
        });
}

void BleatGattChar_Win10::disable_notifications_async(void* context, Void_VoidP_BleatGattCharP_CharP handler) {
    create_task(characteristic->WriteClientCharacteristicConfigurationDescriptorAsync(GattClientCharacteristicConfigurationDescriptorValue::None))
        .then([context, handler, this](GattCommunicationStatus status) {
            if (status == GattCommunicationStatus::Success) {
                characteristic->ValueChanged -= cookie;
                handler(context, this, nullptr);
            } else {
                handler(context, this, BLEAT_GATT_DISABLE_NOTIFY_ERROR);
            }
        });
}

void BleatGattChar_Win10::set_value_changed_handler(void* context, Void_VoidP_BleatGattCharP_UbyteP_Ubyte handler) {
    cookie = characteristic->ValueChanged += ref new TypedEventHandler<GattCharacteristic^, GattValueChangedEventArgs^>([context, handler, this](GattCharacteristic^ sender, GattValueChangedEventArgs^ obj) {
        Array<byte>^ wrapper = ref new Array<byte>(obj->CharacteristicValue->Length);
        CryptographicBuffer::CopyToByteArray(obj->CharacteristicValue, &wrapper);
        handler(context, this, (uint8_t*)wrapper->Data, wrapper->Length);
    });
}

#endif
