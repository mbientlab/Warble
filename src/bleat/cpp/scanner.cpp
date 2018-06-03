/**
 * @copyright MbientLab License
 */

#include "scanner_def.h"
#include "bleat/scanner.h"

using namespace std;

static BleatScanner* scanner = nullptr;
static inline BleatScanner* get_scanner() {
    if (scanner == nullptr) {
        scanner = bleat_scanner_create();
    }
    return scanner;
}

BleatScanner::~BleatScanner() {

}

void bleat_scanner_set_handler(void* context, Void_VoidP_BleatScanResultP handler) {
    get_scanner()->set_handler(context, handler);
}

void bleat_scanner_start(int32_t nopts, const BleatOption* opts) {
    get_scanner()->start(nopts, opts);
}

void bleat_scanner_stop() {
    get_scanner()->stop();
}

const BleatScanMftData* bleat_scan_result_get_manufacturer_data(const BleatScanResult* result, BLEAT_USHORT company_id) {
    auto manufacturers = &((BleppScanPrivateData*) result->private_data)->manufacturer_data;
    auto it = manufacturers->find(company_id);

    return it == manufacturers->end() ? nullptr : &(it->second);
}

int32_t bleat_scan_result_has_service_uuid(const BleatScanResult* result, const char* uuid) {
    return ((BleppScanPrivateData*) result->private_data)->service_uuids.count(uuid);
}