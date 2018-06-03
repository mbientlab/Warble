/**
 * @copyright MbientLab License
 */

#pragma once

#include "bleat/scan_result.h"
#include "bleat/types.h"

#include <string>
#include <unordered_set>
#include <unordered_map>

class BleatScanner {
public:
    virtual ~BleatScanner() = 0;

    virtual void set_handler(void* context, Void_VoidP_BleatScanResultP handler) = 0;
    virtual void start(std::int32_t nopts, const BleatOption* opts) = 0;
    virtual void stop() = 0;
};

struct BleatScanPrivateData {
    std::unordered_set<std::string> service_uuids;
    std::unordered_map<std::uint16_t, BleatScanMftData> manufacturer_data;
};

BleatScanner* bleat_scanner_create();