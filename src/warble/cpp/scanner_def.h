/**
 * @copyright MbientLab License
 */

#pragma once

#include "warble/scan_result.h"
#include "warble/types.h"

#include <string>
#include <unordered_set>
#include <unordered_map>

class WarbleScanner {
public:
    virtual ~WarbleScanner() = 0;

    virtual void set_handler(void* context, FnVoid_VoidP_WarbleScanResultP handler) = 0;
    virtual void start(std::int32_t nopts, const WarbleOption* opts) = 0;
    virtual void stop() = 0;
};

struct WarbleScanPrivateData {
    std::unordered_set<std::string> service_uuids;
    std::unordered_map<std::uint16_t, WarbleScanMftData> manufacturer_data;
};

WarbleScanner* warble_scanner_create();