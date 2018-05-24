#pragma once

#include "types.h"

typedef struct {
    const UBYTE* value;
    INT value_size;
    USHORT company_id;
} BleatScanMftData;

typedef struct {
    const char* mac;
    const char* name;
    const BleatScanMftData* manufacturer_data;
    INT manufacturer_data_size, rssi;
} BleatScanResult;

typedef void(*Void_VoidP_BleatScanResultP)(void* context, const BleatScanResult* result);