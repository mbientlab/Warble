/**
 * @copyright MbientLab License
 * @file gattchar.h
 * @brief Bleat gatt characteristic functions
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
BLEAT_API void bleat_gattchar_write_async(BleatGattChar* obj, const BLEAT_UBYTE* value, BLEAT_UBYTE len, void* context, Void_VoidP_BleatGattCharP_CharP handler);
/**
 * Writes the value to the characteristic without requiring a response from the remote device
 * @param obj           Calling object
 * @param value         Pointer to the first byte to write
 * @param len           Number of bytes to write
 * @param context       Additional data for the callback function
 * @param handler       Callback function that is executed when the async task has completed
 */
BLEAT_API void bleat_gattchar_write_without_resp_async(BleatGattChar* obj, const BLEAT_UBYTE* value, BLEAT_UBYTE len, void* context, Void_VoidP_BleatGattCharP_CharP handler);

/**
 * Reads the current value of the characteristic from the remote device
 * @param obj           Calling object
 * @param context       Additional data for the callback function
 * @param handler       Callback function that is executed when the async task has completed
 */
BLEAT_API void bleat_gattchar_read_async(BleatGattChar* obj, void* context, Void_VoidP_BleatGattCharP_UbyteP_Ubyte_CharP handler);

/**
 * Enables notifications on the characteristic
 * @param obj           Calling object
 * @param context       Additional data for the callback function
 * @param handler       Callback function that is executed when the async task has completed
 */
BLEAT_API void bleat_gattchar_enable_notifications_async(BleatGattChar* obj, void* context, Void_VoidP_BleatGattCharP_CharP handler);
/**
 * Disables notifications on the characteristic
 * @param obj           Calling object
 * @param context       Additional data for the callback function
 * @param handler       Callback function that is executed when the async task has completed
 */
BLEAT_API void bleat_gattchar_disable_notifications_async(BleatGattChar* obj, void* context, Void_VoidP_BleatGattCharP_CharP handler);
/**
 * Sets a handler to listen for characteristic notifications
 * @param obj           Calling object
 * @param context       Additional data for the callback function
 * @param handler       Callback function that is executed when notifications are received
 */
BLEAT_API void bleat_gattchar_set_value_changed_handler(BleatGattChar* obj, void* context, Void_VoidP_BleatGattCharP_UbyteP_Ubyte handler);

/**
 * Gets the string representation of the characteristic's uuid
 * @param obj           Calling object
 * @return String representation of the 128-bit uuid
 */
BLEAT_API const char* bleat_gattchar_get_uuid(const BleatGattChar* obj);
/**
 * Gets the BleatGatt object that the characteristic belongs to
 * @param obj           Calling object
 * @return Pointer to the owning BleatGatt object  
 */
BLEAT_API BleatGatt* bleat_gattchar_get_gatt(const BleatGattChar* obj);
#ifdef __cplusplus
}
#endif