#pragma once

#include "dllmarker.h"
#include "gattchar_fwd.h"

#ifdef __cplusplus
extern "C" {
#endif

BLEAT_API void mbl_bleat_gattchar_write_async(MblBleatGattChar* obj, const uint8_t* value, uint8_t len, void* context, Void_VoidP_MblBleatGattCharP handler);
BLEAT_API void mbl_bleat_gattchar_write_without_resp_async(MblBleatGattChar* obj, const uint8_t* value, uint8_t len, void* context, Void_VoidP_MblBleatGattCharP handler);

BLEAT_API void mbl_bleat_gattchar_read_async(MblBleatGattChar* obj, void* context, Void_VoidP_MblBleatGattCharP_UbyteC_Ubyte handler);

BLEAT_API void mbl_bleat_gattchar_enable_notifications_async(MblBleatGattChar* obj, void* context, Void_VoidP_MblBleatGattCharP handler);
BLEAT_API void mbl_bleat_gattchar_disable_notifications_async(MblBleatGattChar* obj, void* context, Void_VoidP_MblBleatGattCharP handler);
BLEAT_API void mbl_bleat_gattchar_set_value_changed_handler(MblBleatGattChar* obj, void* context, Void_VoidP_MblBleatGattCharP_UbyteC_Ubyte handler);

#ifdef __cplusplus
}
#endif