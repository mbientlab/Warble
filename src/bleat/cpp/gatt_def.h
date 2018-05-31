/**
 * @copyright MbientLab License
 */
#pragma once

#include "bleat/gatt_fwd.h"
#include "bleat/gattchar_fwd.h"

#include <string>

struct BleatGatt {
    virtual ~BleatGatt() = 0;

    virtual void connect_async(void* context, Void_VoidP_BleatGattP_CharP handler) = 0;
    virtual void disconnect() = 0;
    virtual void on_disconnect(void* context, Void_VoidP_BleatGattP_Uint handler) = 0;

    virtual BleatGattChar* find_characteristic(const std::string& uuid) = 0;
    virtual bool service_exists(const std::string& uuid) = 0;
};

BleatGatt* bleatgatt_create(std::int32_t nopts, const BleatOption* opts);