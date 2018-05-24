/**
 * @copyright MbientLab License
 * @file scan_result.h
 * @brief Types for ble scan results
 */
#pragma once

#include "types.h"

typedef struct {
    const BLEAT_UBYTE* value;
    BLEAT_INT value_size;
    BLEAT_USHORT company_id;
} BleatScanMftData;

typedef struct {
    const char* mac;
    const char* name;
    const BleatScanMftData* manufacturer_data;
    BLEAT_INT manufacturer_data_size, rssi;
} BleatScanResult;

typedef void(*Void_VoidP_BleatScanResultP)(void* context, const BleatScanResult* result);