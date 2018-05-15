#pragma once

#include "dllmarker.h"
#include "gattchar_fwd.h"
#include "gatt_fwd.h"

#ifdef __cplusplus
extern "C" {
#endif

BLEAT_API MblBleatGatt* mbl_bleat_gatt_new(const char* mac);
BLEAT_API void mbl_bleat_gatt_free(MblBleatGatt* obj);

BLEAT_API void mbl_bleat_gatt_connect_async(MblBleatGatt* obj, void* context, Void_VoidP_MblBleatGattP handler);
BLEAT_API void mbl_bleat_gatt_disconnect(MblBleatGatt* obj);
BLEAT_API void mbl_bleat_gatt_on_disconnect(MblBleatGatt* obj, void* context, Void_VoidP_MblBleatGattP handler);

BLEAT_API void mbl_bleat_gatt_discover_services_async(MblBleatGatt* obj, void* context, Void_VoidP_MblBleatGattP handler);
BLEAT_API MblBleatGattChar* mbl_bleat_gatt_find_characteristic(MblBleatGatt* obj, const char* uuid);

#ifdef __cplusplus
}
#endif