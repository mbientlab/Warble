/**
 * @copyright MbientLab License
 * @file gattchar.h
 * @brief Bleat gatt characteristic functions
 */
#pragma once

#include "dllmarker.h"
#include "gattchar_fwd.h"
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

BLEAT_API void bleat_gattchar_write_async(BleatGattChar* obj, const BLEAT_UBYTE* value, BLEAT_UBYTE len, void* context, Void_VoidP_BleatGattCharP_CharP handler);
BLEAT_API void bleat_gattchar_write_without_resp_async(BleatGattChar* obj, const BLEAT_UBYTE* value, BLEAT_UBYTE len, void* context, Void_VoidP_BleatGattCharP_CharP handler);

BLEAT_API void bleat_gattchar_read_async(BleatGattChar* obj, void* context, Void_VoidP_BleatGattCharP_UbyteC_Ubyte_CharP handler);

BLEAT_API void bleat_gattchar_enable_notifications_async(BleatGattChar* obj, void* context, Void_VoidP_BleatGattCharP_CharP handler);
BLEAT_API void bleat_gattchar_disable_notifications_async(BleatGattChar* obj, void* context, Void_VoidP_BleatGattCharP_CharP handler);
BLEAT_API void bleat_gattchar_set_value_changed_handler(BleatGattChar* obj, void* context, Void_VoidP_BleatGattCharP_UbyteC_Ubyte handler);

#ifdef __cplusplus
}
#endif