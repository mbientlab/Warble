/**
 * @copyright MbientLab License
 * @file scanner.h
 * @brief BLE device scanning functions
 */
#pragma once

#include "dllmarker.h"
#include "scan_result.h"
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Sets a handler that listens for BLE scan results
 * @param context           Additional data for the callback function
 * @param handler           Callback function that is called everytime advertising data is received
 */
WARBLE_API void warble_scanner_set_handler(void* context, FnVoid_VoidP_WarbleScanResultP handler);
/**
 * Starts a BLE scan
 * @param nopts             Number of options being passed
 * @param opts              Array of config options
 */
WARBLE_API void warble_scanner_start(WARBLE_INT nopts, const WarbleOption* opts);
/**
 * Stops the BLE scan
 */
WARBLE_API void warble_scanner_stop();

/**
 * Extracts the manufacturer data from the ad packet
 * @param result            Calling object
 * @param company_id        ID to look up
 * @return Pointer to the manufacturer data, null if <code>company_id</code> is not found
 */
WARBLE_API const WarbleScanMftData* warble_scan_result_get_manufacturer_data(const WarbleScanResult* result, WARBLE_USHORT company_id);
/**
 * Checks if the device is advertising with the specific service uuid
 * @param result            Calling object
 * @param uuid              String representation of the 128-bit uuid, in all lower case
 * @return 0 if device not advertising with the uuid, non-zero if it is
 */
WARBLE_API WARBLE_INT warble_scan_result_has_service_uuid(const WarbleScanResult* result, const char* uuid);

#ifdef __cplusplus
}
#endif