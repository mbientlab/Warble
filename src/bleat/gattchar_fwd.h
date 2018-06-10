/**
 * @copyright MbientLab License
 * @file gattchar_fwd.h
 * @brief Type declarations for the BleatGattChar functions
 */
#pragma once

#include "types.h"

/**
 * Represents a Bluetooth GATT characteristic
 */
#ifdef __cplusplus
struct BleatGattChar;
#else
typedef struct BleatGattChar BleatGattChar;
#endif

/**
 * 3 parameter function that accepts <code>(void*, BleatGattChar*, const char*)</code> and has no return value
 * @param context           Additional data registered with the callback function
 * @param caller            Object associated with the callback function
 * @param value             Additional data returned to the function
 */
typedef void(*Void_VoidP_BleatGattCharP_CharP)(void* context, BleatGattChar* caller, const char* value);
/**
 * 4 parameter function that accepts <code>(void*, BleatGattChar*, const uint8_t*, uint8_t)</code> and has no return value
 * @param context           Additional data registered with the callback function
 * @param caller            Object associated with the callback function
 * @param value             Pointer to the beginning of a byte array
 * @param length            Number of bytes for the value
 */
typedef void(*Void_VoidP_BleatGattCharP_UbyteP_Ubyte)(void* context, BleatGattChar* caller, const BLEAT_UBYTE* value, BLEAT_UBYTE length);
/**
 * 5 parameter function that accepts <code>(void*, BleatGattChar*, const uint8_t*, uint8_t, const char*)</code> and has no return value
 * @param context           Additional data registered with the callback function
 * @param caller            Object associated with the callback function
 * @param value             Pointer to the beginning of a byte array
 * @param length            Number of bytes for the value
 * @param value2            Additional data returned to the function
 */
typedef void(*Void_VoidP_BleatGattCharP_UbyteP_Ubyte_CharP)(void* context, BleatGattChar* caller, const BLEAT_UBYTE* value, BLEAT_UBYTE length, const char* value2);