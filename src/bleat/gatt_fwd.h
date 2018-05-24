/**
 * @copyright MbientLab License
 * @file gatt_fwd.h
 * @brief Type declarations for the BleatGatt functions
 */
#pragma once

#include "types.h"

#define BLEAT_GATT_STATUS_CONNECT_OK 0
#define BLEAT_GATT_STATUS_CONNECT_TIMEOUT 1
#define BLEAT_GATT_STATUS_CONNECT_GATT_ERROR 2

#ifdef __cplusplus
struct BleatGatt;
#else
typedef struct BleatGatt BleatGatt;
#endif

typedef void(*Void_VoidP_BleatGattP_Uint)(void* context, BleatGatt* caller, UINT value);

typedef struct {
    const char* key;
    const char* value;
} BleatGattOption;