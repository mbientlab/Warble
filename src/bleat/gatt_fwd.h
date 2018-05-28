/**
 * @copyright MbientLab License
 * @file gatt_fwd.h
 * @brief Type declarations for the BleatGatt functions
 */
#pragma once

#include "types.h"

#ifdef __cplusplus
struct BleatGatt;
#else
typedef struct BleatGatt BleatGatt;
#endif

typedef void(*Void_VoidP_BleatGattP_CharP)(void* context, BleatGatt* caller, const char* value);
typedef void(*Void_VoidP_BleatGattP_Uint)(void* context, BleatGatt* caller, BLEAT_UINT value);