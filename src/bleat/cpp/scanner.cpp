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
