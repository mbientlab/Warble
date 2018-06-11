/**
 * @copyright MbientLab License
 */
#pragma once

#include "warble/gatt_fwd.h"
#include "warble/gattchar_fwd.h"

#include <string>

struct WarbleGatt {
    virtual ~WarbleGatt() = 0;

    virtual void connect_async(void* context, FnVoid_VoidP_WarbleGattP_CharP handler) = 0;
    virtual void disconnect() = 0;
    virtual void on_disconnect(void* context, FnVoid_VoidP_WarbleGattP_Int handler) = 0;
    virtual bool is_connected() const = 0;

    virtual WarbleGattChar* find_characteristic(const std::string& uuid) const = 0;
    virtual bool service_exists(const std::string& uuid) const = 0;
};

WarbleGatt* warblegatt_create(std::int32_t nopts, const WarbleOption* opts);