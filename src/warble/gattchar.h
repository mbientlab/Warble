/**
 * @copyright MbientLab License
 * @file gattchar.h
 * @brief Warble gatt characteristic functions
 */
#pragma once

#include "dllmarker.h"
#include "gatt_fwd.h"
#include "gattchar_fwd.h"
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Writes the value to the characteristic, requesting a response from the remote device
 * @param obj           Calling object
 * @param value         Pointer to the first byte to write
 * @param len           Number of bytes to write
 * @param context       Additional data for the callback function
 * @param handler       Callback function that is executed when the async task has completed
 */
WARBLE_API void warble_gattchar_write_async(WarbleGattChar* obj, const WARBLE_UBYTE* value, WARBLE_UBYTE len, void* context, FnVoid_VoidP_WarbleGattCharP_CharP handler);
/**
 * Writes the value to the characteristic without requiring a response from the remote device
 * @param obj           Calling object
 * @param value         Pointer to the first byte to write
 * @param len           Number of bytes to write
 * @param context       Additional data for the callback function
 * @param handler       Callback function that is executed when the async task has completed
 */
WARBLE_API void warble_gattchar_write_without_resp_async(WarbleGattChar* obj, const WARBLE_UBYTE* value, WARBLE_UBYTE len, void* context, FnVoid_VoidP_WarbleGattCharP_CharP handler);

/**
 * Reads the current value of the characteristic from the remote device
 * @param obj           Calling object
 * @param context       Additional data for the callback function
 * @param handler       Callback function that is executed when the async task has completed
 */
WARBLE_API void warble_gattchar_read_async(WarbleGattChar* obj, void* context, FnVoid_VoidP_WarbleGattCharP_UbyteP_Ubyte_CharP handler);

/**
 * Enables notifications on the characteristic
 * @param obj           Calling object
 * @param context       Additional data for the callback function
 * @param handler       Callback function that is executed when the async task has completed
 */
WARBLE_API void warble_gattchar_enable_notifications_async(WarbleGattChar* obj, void* context, FnVoid_VoidP_WarbleGattCharP_CharP handler);
/**
 * Disables notifications on the characteristic
 * @param obj           Calling object
 * @param context       Additional data for the callback function
 * @param handler       Callback function that is executed when the async task has completed
 */
WARBLE_API void warble_gattchar_disable_notifications_async(WarbleGattChar* obj, void* context, FnVoid_VoidP_WarbleGattCharP_CharP handler);
/**
 * Sets a handler to listen for characteristic notifications
 * @param obj           Calling object
 * @param context       Additional data for the callback function
 * @param handler       Callback function that is executed when notifications are received
 */
WARBLE_API void warble_gattchar_on_notification_received(WarbleGattChar* obj, void* context, FnVoid_VoidP_WarbleGattCharP_UbyteP_Ubyte handler);

/**
 * Gets the string representation of the characteristic's uuid
 * @param obj           Calling object
 * @return String representation of the 128-bit uuid
 */
WARBLE_API const char* warble_gattchar_get_uuid(const WarbleGattChar* obj);
/**
 * Gets the WarbleGatt object that the characteristic belongs to
 * @param obj           Calling object
 * @return Pointer to the owning WarbleGatt object  
 */
WARBLE_API WarbleGatt* warble_gattchar_get_gatt(const WarbleGattChar* obj);
#ifdef __cplusplus
}
#endif