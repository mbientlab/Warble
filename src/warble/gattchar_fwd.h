/**
 * @copyright MbientLab License
 * @file gattchar_fwd.h
 * @brief Type declarations for the WarbleGattChar functions
 */
#pragma once

#include "types.h"

/**
 * Represents a Bluetooth GATT characteristic
 */
#ifdef __cplusplus
struct WarbleGattChar;
#else
typedef struct WarbleGattChar WarbleGattChar;
#endif

/**
 * 3 parameter function that accepts <code>(void*, WarbleGattChar*, const char*)</code> and has no return value
 * @param context           Additional data registered with the callback function
 * @param caller            Object associated with the callback function
 * @param value             Additional data returned to the function
 */
typedef void(*FnVoid_VoidP_WarbleGattCharP_CharP)(void* context, WarbleGattChar* caller, const char* value);
/**
 * 4 parameter function that accepts <code>(void*, WarbleGattChar*, const uint8_t*, uint8_t)</code> and has no return value
 * @param context           Additional data registered with the callback function
 * @param caller            Object associated with the callback function
 * @param value             Pointer to the beginning of a byte array
 * @param length            Number of bytes for the value
 */
typedef void(*FnVoid_VoidP_WarbleGattCharP_UbyteP_Ubyte)(void* context, WarbleGattChar* caller, const WARBLE_UBYTE* value, WARBLE_UBYTE length);
/**
 * 5 parameter function that accepts <code>(void*, WarbleGattChar*, const uint8_t*, uint8_t, const char*)</code> and has no return value
 * @param context           Additional data registered with the callback function
 * @param caller            Object associated with the callback function
 * @param value             Pointer to the beginning of a byte array
 * @param length            Number of bytes for the value
 * @param value2            Additional data returned to the function
 */
typedef void(*FnVoid_VoidP_WarbleGattCharP_UbyteP_Ubyte_CharP)(void* context, WarbleGattChar* caller, const WARBLE_UBYTE* value, WARBLE_UBYTE length, const char* value2);