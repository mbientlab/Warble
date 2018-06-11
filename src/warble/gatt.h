/**
 * @copyright MbientLab License
 * @file gatt.h
 * @brief Functions for the WarbleGatt object
 */
#pragma once

#include "dllmarker.h"
#include "gattchar_fwd.h"
#include "gatt_fwd.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Creates a WarbleGatt object
 * @param mac           mac address of the remote device e.g. CB:B7:49:BF:27:33
 * @return Pointer to the newly created object
 */
WARBLE_API WarbleGatt* warble_gatt_create(const char* mac);
/**
 * Creates a WarbleGatt object
 * @param mac           mac address of the remote device e.g. CB:B7:49:BF:27:33
 * @return Pointer to the newly created object
 */
WARBLE_API WarbleGatt* warble_gatt_create_with_options(WARBLE_INT nopts, const WarbleOption* opts);
/**
 * Frees the memory allocated for the WarbleGatt object
 * @param obj           Object to delete
 */
WARBLE_API void warble_gatt_delete(WarbleGatt* obj);

/**
 * Connects to the remote device
 * @param obj           Calling object
 * @param context       Additional data for the callback function
 * @param handler       Callback function that is executed when the connect task is completed
 */
WARBLE_API void warble_gatt_connect_async(WarbleGatt* obj, void* context, FnVoid_VoidP_WarbleGattP_CharP handler);
/**
 * Disconnects from the remot device.  The callback function set in <code>warble_gatt_on_disconnect</code> will be called 
 * after all resources are freed
 * @param obj           Calling object
 */
WARBLE_API void warble_gatt_disconnect(WarbleGatt* obj);
/**
 * Sets a handler to listen for disconnect events
 * @param obj           Calling object
 * @param context       Additional data for the callback function
 * @param handler       Callback function that is executed when a disconnect event is received
 */
WARBLE_API void warble_gatt_on_disconnect(WarbleGatt* obj, void* context, FnVoid_VoidP_WarbleGattP_Int handler);
/**
 * Checks the current connection status
 * @param obj           Calling object
 * @return 0 if there is no active connect, non-zero if connected
 */
WARBLE_API WARBLE_INT warble_gatt_is_connected(const WarbleGatt* obj);

/**
 * Checks if a GATT characteristic exists with the uuid
 * @param obj           Calling object
 * @param uuid          128-bit string representation of the uuid
 * @return WarbleGattChar pointer if characteristic exists, null otherwise
 */
WARBLE_API WarbleGattChar* warble_gatt_find_characteristic(const WarbleGatt* obj, const char* uuid);
/**
 * Checks if a GATT service exists with the uuid
 * @param obj           Calling object
 * @param uuid          128-bit string representation of the uuid
 * @return 0 if there service does not exists, non-zero otherwise
 */
WARBLE_API WARBLE_INT warble_gatt_has_service(const WarbleGatt* obj, const char* uuid);

#ifdef __cplusplus
}
#endif