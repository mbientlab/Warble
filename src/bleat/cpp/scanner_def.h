/**
 * @copyright MbientLab License
 */

#pragma once

#include "bleat/scan_result.h"
#include "bleat/types.h"

class BleatScanner {
public:
    virtual ~BleatScanner() = 0;

    virtual void set_handler(void* context, Void_VoidP_BleatScanResultP handler) = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
};