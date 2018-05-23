#pragma once

#ifdef __cplusplus

#define BLEAT_GATT_STATUS_CONNECT_OK 0
#define BLEAT_GATT_STATUS_CONNECT_TIMEOUT 1
#define BLEAT_GATT_STATUS_CONNECT_GATT_ERROR 2

#include <cstdint>
struct BleatGatt;
#define UINT std::uint32_t

#else

#include <stdint.h>
typedef struct BleatGatt BleatGatt;
#define UINT uint32_t

#endif

typedef void(*Void_VoidP_BleatGattP_Uint)(void* context, BleatGatt* caller, UINT value);

typedef struct {
    const char* key;
    const char* value;
} BleatGattOption;