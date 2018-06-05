/**
 * @copyright MbientLab License
 * @file gattchar.h
 * @brief Bleat gatt characteristic functions
 */
#pragma once

#include "dllmarker.h"
#include "gatt_fwd.h"
#include "gattchar_fwd.h"
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

BLEAT_API void bleat_gattchar_write_async(BleatGattChar* obj, const BLEAT_UBYTE* value, BLEAT_UBYTE len, void* context, Void_VoidP_BleatGattCharP_CharP handler);
BLEAT_API void bleat_gattchar_write_without_resp_async(BleatGattChar* obj, const BLEAT_UBYTE* value, BLEAT_UBYTE len, void* context, Void_VoidP_BleatGattCharP_CharP handler);

BLEAT_API void bleat_gattchar_read_async(BleatGattChar* obj, void* context, Void_VoidP_BleatGattCharP_UbyteP_Ubyte_CharP handler);

BLEAT_API void bleat_gattchar_enable_notifications_async(BleatGattChar* obj, void* context, Void_VoidP_BleatGattCharP_CharP handler);
BLEAT_API void bleat_gattchar_disable_notifications_async(BleatGattChar* obj, void* context, Void_VoidP_BleatGattCharP_CharP handler);
BLEAT_API void bleat_gattchar_set_value_changed_handler(BleatGattChar* obj, void* context, Void_VoidP_BleatGattCharP_UbyteP_Ubyte handler);

BLEAT_API const char* bleat_gattchar_get_uuid(const BleatGattChar* obj);
BLEAT_API BleatGatt* bleat_gattchar_get_gatt(const BleatGattChar* obj);
#ifdef __cplusplus
}
#endif