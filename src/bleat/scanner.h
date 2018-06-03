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

BLEAT_API void bleat_scanner_set_handler(void* context, Void_VoidP_BleatScanResultP handler);
BLEAT_API void bleat_scanner_start(BLEAT_INT nopts, const BleatOption* opts);
BLEAT_API void bleat_scanner_stop();

BLEAT_API const BleatScanMftData* bleat_scan_result_get_manufacturer_data(const BleatScanResult* result, BLEAT_USHORT company_id);
BLEAT_API BLEAT_INT bleat_scan_result_has_service_uuid(const BleatScanResult* result, const char* uuid);

#ifdef __cplusplus
}
#endif