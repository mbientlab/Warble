/**
 * @copyright MbientLab License
 */
#pragma once

#ifdef API_WIN10

#include "gatt_def.h"
#include "gattchar_def.h"

#include <ppltasks.h>
#include <unordered_map>
#include <Windows.Devices.Bluetooth.h>

struct Hasher {
    size_t operator() (Platform::Guid key) const {
        return key.GetHashCode();
    }
};
struct EqualFn {
    bool operator() (Platform::Guid t1, Platform::Guid t2) const {
        return t1.Equals(t2);
    }
};

struct BleatGattChar_Win10;

struct BleatGatt_Win10 : public BleatGatt {
    BleatGatt_Win10(int nopts, const BleatGattOption* opts);
    virtual ~BleatGatt_Win10();

    virtual void connect_async(void* context, Void_VoidP_BleatGattP_Uint handler);
    virtual void disconnect();
    virtual void on_disconnect(void* context, Void_VoidP_BleatGattP_Uint handler);

    virtual BleatGattChar* find_characteristic(const std::string& uuid);

private:
    void cleanup();

    const char* mac;

    void *on_disconnect_context;
    Void_VoidP_BleatGattP_Uint on_disconnect_handler;
    
    concurrency::task<void> discover_task, connect_task;

    Windows::Devices::Bluetooth::BluetoothLEDevice^ device;
    Windows::Foundation::EventRegistrationToken cookie;
    std::unordered_map<Platform::Guid, BleatGattChar_Win10*, Hasher, EqualFn> characteristics;
};

struct BleatGattChar_Win10 : public BleatGattChar {
    BleatGattChar_Win10(Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristic^ characteristic);

    virtual ~BleatGattChar_Win10();

    virtual void write_async(const std::uint8_t* value, std::uint8_t len, void* context, Void_VoidP_BleatGattCharP handler);
    virtual void write_without_resp_async(const std::uint8_t* value, std::uint8_t len, void* context, Void_VoidP_BleatGattCharP handler);

    virtual void read_async(void* context, Void_VoidP_BleatGattCharP_UbyteC_Ubyte handler);

    virtual void enable_notifications_async(void* context, Void_VoidP_BleatGattCharP handler);
    virtual void disable_notifications_async(void* context, Void_VoidP_BleatGattCharP handler);
    virtual void set_value_changed_handler(void* context, Void_VoidP_BleatGattCharP_UbyteC_Ubyte handler);

private:
    inline void write_inner_async(Windows::Devices::Bluetooth::GenericAttributeProfile::GattWriteOption option, const std::uint8_t* value, std::uint8_t len, void* context, Void_VoidP_BleatGattCharP handler) {
        Platform::Array<byte>^ wrapper = ref new Platform::Array<byte>(len);
        for (uint8_t i = 0; i < len; i++) {
            wrapper[i] = value[i];
        }

        concurrency::create_task(characteristic->WriteValueAsync(Windows::Security::Cryptography::CryptographicBuffer::CreateFromByteArray(wrapper), option))
            .then([context, handler, this](Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus status) {
            handler(context, this);
        });
    }

    Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristic^ characteristic;
    Windows::Foundation::EventRegistrationToken cookie;
};

#endif
