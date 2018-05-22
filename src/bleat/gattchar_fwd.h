#pragma once

#ifdef __cplusplus

#include <cstdint>
struct BleatGattChar;
#define UBYTE std::uint8_t

#else

#include <stdint.h>
typedef struct BleatGattChar BleatGattChar;
#define UBYTE uint8_t

#endif

typedef void(*Void_VoidP_BleatGattCharP)(void* context, BleatGattChar* caller);
typedef void(*Void_VoidP_BleatGattCharP_UbyteC_Ubyte)(void* context, BleatGattChar* caller, const UBYTE* value, UBYTE length);