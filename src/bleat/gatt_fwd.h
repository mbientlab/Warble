#pragma once

#ifdef __cplusplus
struct MblBleatGatt;
#else
typedef struct MblBleatGatt MblBleatGatt;
#endif

typedef void(*Void_VoidP_MblBleatGattP)(void* context, MblBleatGatt* caller);