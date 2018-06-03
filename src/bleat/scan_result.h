/**
 * @copyright MbientLab License
 * @file scan_result.h
 * @brief Types for ble scan results
 */
#pragma once

#include "types.h"

typedef struct {
    const BLEAT_UBYTE* value;
    BLEAT_UINT value_size;
} BleatScanMftData;

typedef struct {
    const char* mac;
    const char* name;
    BLEAT_INT rssi;
    void* private_data;
} BleatScanResult;

typedef void(*Void_VoidP_BleatScanResultP)(void* context, const BleatScanResult* result);