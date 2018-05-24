#include "scanner_def.h"
#include "bleat/scanner.h"

#include "blepp_scanner.h"
static BleatScanner* scanner = new BleatScanner_Blepp();

BleatScanner::~BleatScanner() {

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
