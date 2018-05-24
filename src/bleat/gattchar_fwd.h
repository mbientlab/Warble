/**
 * @copyright MbientLab License
 * @file gattchar_fwd.h
 * @brief Type declarations for the BleatGattChar functions
 */
#pragma once

#include "types.h"

#ifdef __cplusplus
struct BleatGattChar;
#else
typedef struct BleatGattChar BleatGattChar;
#endif

typedef void(*Void_VoidP_BleatGattCharP)(void* context, BleatGattChar* caller);
typedef void(*Void_VoidP_BleatGattCharP_UbyteC_Ubyte)(void* context, BleatGattChar* caller, const BLEAT_UBYTE* value, BLEAT_UBYTE length);