/**
 * @copyright MbientLab License
 * @file gatt.h
 * @brief Functions for the BleatGatt object
 */
#pragma once

#include "dllmarker.h"
#include "gattchar_fwd.h"
#include "gatt_fwd.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Creates a BleatGatt object
 * @param mac           mac address of the remote device e.g. CB:B7:49:BF:27:33
 * @return Pointer to the newly created object
 */
BLEAT_API BleatGatt* bleat_gatt_create(const char* mac);
/**
 * Creates a BleatGatt object
 * @param mac           mac address of the remote device e.g. CB:B7:49:BF:27:33
 * @return Pointer to the newly created object
 */
BLEAT_API BleatGatt* bleat_gatt_create_with_options(BLEAT_INT nopts, const BleatOption* opts);
/**
 * Frees the memory allocated for the BleatGatt object
 * @param obj           Object to delete
 */
BLEAT_API void bleat_gatt_delete(BleatGatt* obj);

/**
 * Connects to the remote device
 * @param obj           Calling object
 * @param context       Additional data for the callback function
 * @param handler       Callback function that is executed when the connect task is completed
 */
BLEAT_API void bleat_gatt_connect_async(BleatGatt* obj, void* context, Void_VoidP_BleatGattP_CharP handler);
/**
 * Disconnects from the remot device.  The callback function set in <code>bleat_gatt_on_disconnect</code> will be called 
 * after all resources are freed
 * @param obj           Calling object
 */
BLEAT_API void bleat_gatt_disconnect(BleatGatt* obj);
/**
 * Sets a handler to listen for disconnect events
 * @param obj           Calling object
 * @param context       Additional data for the callback function
 * @param handler       Callback function that is executed when a disconnect event is received
 */
BLEAT_API void bleat_gatt_on_disconnect(BleatGatt* obj, void* context, Void_VoidP_BleatGattP_Int handler);
/**
 * Checks the current connection status
 * @param obj           Calling object
 * @return 0 if there is no active connect, non-zero if connected
 */
BLEAT_API BLEAT_INT bleat_gatt_is_connected(const BleatGatt* obj);

/**
 * Checks if a GATT characteristic exists with the uuid
 * @param obj           Calling object
 * @param uuid          128-bit string representation of the uuid
 * @return BleatGattChar pointer if characteristic exists, null otherwise
 */
BLEAT_API BleatGattChar* bleat_gatt_find_characteristic(const BleatGatt* obj, const char* uuid);
/**
 * Checks if a GATT service exists with the uuid
 * @param obj           Calling object
 * @param uuid          128-bit string representation of the uuid
 * @return 0 if there service does not exists, non-zero otherwise
 */
BLEAT_API BLEAT_INT bleat_gatt_has_service(const BleatGatt* obj, const char* uuid);

#ifdef __cplusplus
}
#endif