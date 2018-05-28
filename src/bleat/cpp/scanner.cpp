/**
 * @copyright MbientLab License
 */

#include "scanner_def.h"
#include "bleat/scanner.h"

#ifdef API_BLEPP
#include <functional>
#include <string>
#include <unordered_map>
#include "blepp_scanner.h"

static BleatScanner_Blepp* scanner = new BleatScanner_Blepp();
#elif API_WIN10
#include "win10_scanner.h"
static BleatScanner_Win10* scanner = new BleatScanner_Win10();
#endif

using namespace std;

BleatScanner::~BleatScanner() {

}

void bleat_scanner_configure(BLEAT_INT nopts, const BleatOption* opts) {
#ifdef API_BLEPP
    const char *device = nullptr;
    unordered_map<string, function<void(const char*)>> arg_processors = {
        { "hci", [&device](const char* value) { device = value; } }
    };

    void* context = scanner->scan_result_context;
    Void_VoidP_BleatScanResultP handler = scanner->scan_result_handler;
    delete scanner;

    scanner = device == nullptr ? new BleatScanner_Blepp() : new BleatScanner_Blepp(device);
    scanner->scan_result_context = context;
    scanner->scan_result_handler = handler;
#endif
}

void bleat_scanner_set_handler(void* context, Void_VoidP_BleatScanResultP handler) {
    scanner->set_handler(context, handler);
}

void bleat_scanner_start() {
    scanner->start();
}

void bleat_scanner_stop() {
    scanner->stop();
}
