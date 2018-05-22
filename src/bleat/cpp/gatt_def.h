#pragma once

#include "bleat/gatt_fwd.h"
#include "bleat/gattchar_fwd.h"

#include <string>

struct BleatGatt {
    virtual ~BleatGatt() = 0;

    virtual void connect_async(void* context, Void_VoidP_BleatGattP_Uint handler) = 0;
    virtual void disconnect() = 0;
    virtual void on_disconnect(void* context, Void_VoidP_BleatGattP_Uint handler) = 0;

    virtual BleatGattChar* find_characteristic(const std::string& uuid) = 0;
};