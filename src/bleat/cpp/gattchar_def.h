/**
 * @copyright MbientLab License
 */
#pragma once

#include "bleat/gattchar_fwd.h"

struct BleatGattChar {
    virtual ~BleatGattChar() = 0;

    virtual void write_async(const uint8_t* value, uint8_t len, void* context, Void_VoidP_BleatGattCharP handler) = 0;
    virtual void write_without_resp_async(const uint8_t* value, uint8_t len, void* context, Void_VoidP_BleatGattCharP handler) = 0;

    virtual void read_async(void* context, Void_VoidP_BleatGattCharP_UbyteC_Ubyte handler) = 0;

    virtual void enable_notifications_async(void* context, Void_VoidP_BleatGattCharP handler) = 0;
    virtual void disable_notifications_async(void* context, Void_VoidP_BleatGattCharP handler) = 0;
    virtual void set_value_changed_handler(void* context, Void_VoidP_BleatGattCharP_UbyteC_Ubyte handler) = 0;
};