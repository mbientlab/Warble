#pragma once

#ifdef __cplusplus
struct BleatGatt;
#else
typedef struct BleatGatt BleatGatt;
#endif

typedef void(*Void_VoidP_BleatGattP)(void* context, BleatGatt* caller);