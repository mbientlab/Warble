/**
 * @copyright MbientLab License
 */
#pragma once

#include "bleat/gattchar_fwd.h"

struct BleatGattChar {
    virtual ~BleatGattChar() = 0;

    virtual void write_async(const std::uint8_t* value, std::uint8_t len, void* context, Void_VoidP_BleatGattCharP_CharP handler) = 0;
    virtual void write_without_resp_async(const std::uint8_t* value, std::uint8_t len, void* context, Void_VoidP_BleatGattCharP_CharP handler) = 0;

    virtual void read_async(void* context, Void_VoidP_BleatGattCharP_UbyteC_Ubyte_CharP handler) = 0;

    virtual void enable_notifications_async(void* context, Void_VoidP_BleatGattCharP_CharP handler) = 0;
    virtual void disable_notifications_async(void* context, Void_VoidP_BleatGattCharP_CharP handler) = 0;
    virtual void set_value_changed_handler(void* context, Void_VoidP_BleatGattCharP_UbyteC_Ubyte handler) = 0;
};