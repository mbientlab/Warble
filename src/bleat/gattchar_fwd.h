#pragma once

#ifdef __cplusplus

#include <cstdint>
struct BleatGattChar;
typedef void(*Void_VoidP_BleatGattCharP_UbyteC_Ubyte)(void* context, BleatGattChar* caller, const std::uint8_t* value, std::uint8_t length);

#else

#include <stdint.h>
typedef struct BleatGattChar BleatGattChar;
typedef void(*Void_VoidP_BleatGattCharP_UbyteC_Ubyte)(void* context, BleatGattChar* caller, const uint8_t* value, uint8_t length);

#endif

typedef void(*Void_VoidP_BleatGattCharP)(void* context, BleatGattChar* caller);

