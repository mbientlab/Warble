/**
 * @copyright MbientLab License
 * @file gatt_fwd.h
 * @brief Type declarations for the WarbleGatt functions
 */
#pragma once

#include "types.h"

/**
 * Main object for warble, representing BLE GATT operations
 */
#ifdef __cplusplus
struct WarbleGatt;
#else
typedef struct WarbleGatt WarbleGatt;
#endif

/**
 * 3 parameter function that accepts <code>(void*, WarbleGatt*, const char*)</code> and has no return value
 * @param context           Additional data registered with the callback function
 * @param caller            Object associated with the callback function
 * @param value             Additional data returned to the function
 */
typedef void(*FnVoid_VoidP_WarbleGattP_CharP)(void* context, WarbleGatt* caller, const char* value);
/**
 * 3 parameter function that accepts <code>(void*, WarbleGatt*, int)</code> and has no return value
 * @param context           Additional data registered with the callback function
 * @param caller            Object associated with the callback function
 * @param value             Additional data returned to the function
 */
typedef void(*FnVoid_VoidP_WarbleGattP_Int)(void* context, WarbleGatt* caller, WARBLE_INT value);