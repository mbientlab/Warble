/**
 * @copyright MbientLab License
 */
#pragma once

#ifdef API_BLEPP

#include "scanner_def.h"

#include "blepp/blestatemachine.h"
#include "blepp/lescan.h"

#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>

class BleatScanner_Blepp : public BleatScanner {
public:
    BleatScanner_Blepp();
    BleatScanner_Blepp(const char* device);
    virtual ~BleatScanner_Blepp();

    virtual void set_handler(void* context, Void_VoidP_BleatScanResultP handler);
    virtual void start();
    virtual void stop();

    void* scan_result_context;
    Void_VoidP_BleatScanResultP scan_result_handler;

private:
    BLEPP::HCIScanner scanner;
    std::unordered_map<std::string, std::string> seen_devices;

    std::thread scan_thread;
    std::condition_variable scan_cv;
    std::mutex cv_m;
    std::unique_lock<std::mutex> cv_lock;
    bool terminate_scan;
};

#endif
