/**
 * @copyright MbientLab License
 */

#include "scanner_def.h"
#include "bleat/scanner.h"

#ifdef API_BLEPP
#include "blepp_scanner.h"
static BleatScanner_Blepp scanner;
#elif API_WIN10
#include "win10_scanner.h"
static BleatScanner_Win10 scanner;
#endif

BleatScanner::~BleatScanner() {

}

void bleat_scanner_set_handler(void* context, Void_VoidP_BleatScanResultP handler) {
    scanner.set_handler(context, handler);
}

void bleat_scanner_start() {
    scanner.start();
}

void bleat_scanner_stop() {
    scanner.stop();
}
