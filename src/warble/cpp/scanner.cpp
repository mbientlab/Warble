/**
 * @copyright MbientLab License
 */

#include "scanner_def.h"
#include "warble/scanner.h"

using namespace std;

static WarbleScanner* scanner = nullptr;
static inline WarbleScanner* get_scanner() {
    if (scanner == nullptr) {
        scanner = warble_scanner_create();
    }
    return scanner;
}

WarbleScanner::~WarbleScanner() {

}

void warble_scanner_set_handler(void* context, FnVoid_VoidP_WarbleScanResultP handler) {
    get_scanner()->set_handler(context, handler);
}

void warble_scanner_start(int32_t nopts, const WarbleOption* opts) {
    get_scanner()->start(nopts, opts);
}

void warble_scanner_stop() {
    get_scanner()->stop();
}

const WarbleScanMftData* warble_scan_result_get_manufacturer_data(const WarbleScanResult* result, WARBLE_USHORT company_id) {
    auto manufacturers = &((WarbleScanPrivateData*) result->private_data)->manufacturer_data;
    auto it = manufacturers->find(company_id);

    return it == manufacturers->end() ? nullptr : &(it->second);
}

int32_t warble_scan_result_has_service_uuid(const WarbleScanResult* result, const char* uuid) {
    return ((WarbleScanPrivateData*) result->private_data)->service_uuids.count(uuid);
}