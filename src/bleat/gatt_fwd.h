/**
 * @copyright MbientLab License
 * @file gatt_fwd.h
 * @brief Type declarations for the BleatGatt functions
 */
#pragma once

#include "types.h"

/**
 * Main object for bleat, representing BLE GATT operations
 */
#ifdef __cplusplus
struct BleatGatt;
#else
typedef struct BleatGatt BleatGatt;
#endif

/**
 * 3 parameter function that accepts <code>(void*, BleatGatt*, const char*)</code> and has no return value
 * @param context           Additional data registered with the callback function
 * @param caller            Object associated with the callback function
 * @param value             Additional data returned to the function
 */
typedef void(*Void_VoidP_BleatGattP_CharP)(void* context, BleatGatt* caller, const char* value);
/**
 * 3 parameter function that accepts <code>(void*, BleatGatt*, int)</code> and has no return value
 * @param context           Additional data registered with the callback function
 * @param caller            Object associated with the callback function
 * @param value             Additional data returned to the function
 */
typedef void(*Void_VoidP_BleatGattP_Int)(void* context, BleatGatt* caller, BLEAT_INT value);