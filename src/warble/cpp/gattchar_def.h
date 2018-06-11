/**
 * @copyright MbientLab License
 */
#pragma once

#include "warble/gattchar_fwd.h"

struct WarbleGattChar {
    virtual ~WarbleGattChar() = 0;

    virtual void write_async(const std::uint8_t* value, std::uint8_t len, void* context, FnVoid_VoidP_WarbleGattCharP_CharP handler) = 0;
    virtual void write_without_resp_async(const std::uint8_t* value, std::uint8_t len, void* context, FnVoid_VoidP_WarbleGattCharP_CharP handler) = 0;

    virtual void read_async(void* context, FnVoid_VoidP_WarbleGattCharP_UbyteP_Ubyte_CharP handler) = 0;

    virtual void enable_notifications_async(void* context, FnVoid_VoidP_WarbleGattCharP_CharP handler) = 0;
    virtual void disable_notifications_async(void* context, FnVoid_VoidP_WarbleGattCharP_CharP handler) = 0;
    virtual void on_notification_received(void* context, FnVoid_VoidP_WarbleGattCharP_UbyteP_Ubyte handler) = 0;

    virtual const char* get_uuid() const = 0;
    virtual WarbleGatt* get_gatt() const = 0;
};