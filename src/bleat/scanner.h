#pragma once

#include "dllmarker.h"
#include "scan_result.h"

#ifdef __cplusplus
extern "C" {
#endif

BLEAT_API void bleat_scanner_set_handler(void* context, Void_VoidP_BleatScanResultP handler);
BLEAT_API void bleat_scanner_start();
BLEAT_API void bleat_scanner_stop();

#ifdef __cplusplus
}
#endif