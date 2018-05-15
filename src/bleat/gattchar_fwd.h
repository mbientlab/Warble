#pragma once

#ifdef __cplusplus

#include <cstdint>
struct MblBleatGattChar;
typedef void(*Void_VoidP_MblBleatGattCharP_UbyteC_Ubyte)(void* context, MblBleatGattChar* caller, const std::uint8_t* value, std::uint8_t length);

#else

#include <stdint.h>
typedef struct MblBleatGattChar MblBleatGattChar;
typedef void(*Void_VoidP_MblBleatGattCharP_UbyteC_Ubyte)(void* context, MblBleatGattChar* caller, const uint8_t* value, uint8_t length);

#endif

typedef void(*Void_VoidP_MblBleatGattCharP)(void* context, MblBleatGattChar* caller);

