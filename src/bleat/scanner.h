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
BLEAT_API void bleat_scanner_set_handler(void* context, Void_VoidP_BleatScanResultP handler);
/**
 * Starts a BLE scan
 * @param nopts             Number of options being passed
 * @param opts              Array of config options
 */
BLEAT_API void bleat_scanner_start(BLEAT_INT nopts, const BleatOption* opts);
/**
 * Stops the BLE scan
 */
BLEAT_API void bleat_scanner_stop();

/**
 * Extracts the manufacturer data from the ad packet
 * @param result            Calling object
 * @param company_id        ID to look up
 * @return Pointer to the manufacturer data, null if <code>company_id</code> is not found
 */
BLEAT_API const BleatScanMftData* bleat_scan_result_get_manufacturer_data(const BleatScanResult* result, BLEAT_USHORT company_id);
/**
 * Checks if the device is advertising with the specific service uuid
 * @param result            Calling object
 * @param uuid              String representation of the 128-bit uuid, in all lower case
 * @return 0 if device not advertising with the uuid, non-zero if it is
 */
BLEAT_API BLEAT_INT bleat_scan_result_has_service_uuid(const BleatScanResult* result, const char* uuid);

#ifdef __cplusplus
}
#endif