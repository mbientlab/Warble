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

typedef void(*Void_VoidP_BleatGattCharP_CharP)(void* context, BleatGattChar* caller, const char* value);
typedef void(*Void_VoidP_BleatGattCharP_UbyteP_Ubyte)(void* context, BleatGattChar* caller, const BLEAT_UBYTE* value, BLEAT_UBYTE length);
typedef void(*Void_VoidP_BleatGattCharP_UbyteP_Ubyte_CharP)(void* context, BleatGattChar* caller, const BLEAT_UBYTE* value, BLEAT_UBYTE length, const char* value2);