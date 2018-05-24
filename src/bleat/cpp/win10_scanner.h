#pragma once

#include "scanner_def.h"

#include <cstdint>
#include <Windows.Devices.Bluetooth.h>
#include <unordered_map>

class BleatScanner_Win10 : public BleatScanner {
public:
    BleatScanner_Win10();
    virtual ~BleatScanner_Win10();

    virtual void set_handler(void* context, Void_VoidP_BleatScanResultP handler);
    virtual void start();
    virtual void stop();

private:
    void* scan_result_context;
    Void_VoidP_BleatScanResultP scan_result_handler;
    
    std::unordered_map<std::uint64_t, std::string> seen_devices;
    Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementWatcher^ watcher;
};