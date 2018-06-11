/**
 * @copyright MbientLab License
 * @file scan_result.h
 * @brief Types for ble scan results
 */
#pragma once

#include "types.h"

/**
 * Portion of the scan response representing manufacturer data
 */
typedef struct {
    const WARBLE_UBYTE* value;       ///< Byte array value of the data
    WARBLE_UINT value_size;          ///< Number of bytes
} WarbleScanMftData;

/**
 * Information received from a discovered BLE device
 */
typedef struct {
    const char* mac;                ///< Mac address of the advertising device
    const char* name;               ///< Advertising name
    WARBLE_INT rssi;                 ///< Received signal strength
    void* private_data;             ///< Additional data received from the ad packet
} WarbleScanResult;

/**
 * 2 parameter function that accepts `(void*, WarbleScanResult*)` with no return type
 * @param context               Additional data that was registered with the function
 * @param result                Advertising data received from a remote device
 */
typedef void(*FnVoid_VoidP_WarbleScanResultP)(void* context, const WarbleScanResult* result);