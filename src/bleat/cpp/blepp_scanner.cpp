/**
 * @copyright MbientLab License
 */
#ifdef API_BLEPP

#include "scanner_def.h"

#include "blepp_utils.h"
#include "blepp/blestatemachine.h"
#include "blepp/lescan.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <thread>
#include <unistd.h>
#include <unordered_map>

using namespace std;
using namespace BLEPP;

class BleatScanner_Blepp : public BleatScanner {
public:
    BleatScanner_Blepp();
    virtual ~BleatScanner_Blepp();

    virtual void set_handler(void* context, Void_VoidP_BleatScanResultP handler);
    virtual void start(BLEAT_INT nopts, const BleatOption* opts);
    virtual void stop();

private:
    BLEPP::HCIScanner* scanner;
    unordered_map<string, BleatScanPrivateData> seen_devices;
    unordered_map<string, string> device_names;

    void* scan_result_context;
    Void_VoidP_BleatScanResultP scan_result_handler;
    std::thread scan_thread;
    bool terminate_scan;
};

BleatScanner* bleat_scanner_create() {
    return new BleatScanner_Blepp();
}

BleatScanner_Blepp::BleatScanner_Blepp() : scanner(nullptr), scan_result_context(nullptr), scan_result_handler(nullptr) {
}

BleatScanner_Blepp::~BleatScanner_Blepp() {
    stop();
}

void BleatScanner_Blepp::set_handler(void* context, Void_VoidP_BleatScanResultP handler) {
    scan_result_context = context;
    scan_result_handler = handler;
}

void BleatScanner_Blepp::start(int32_t nopts, const BleatOption* opts) {
    if (scanner != nullptr) {
        return;
    }

    const char *device = "";
    unordered_map<string, function<void(const char*)>> arg_processors = {
        { "hci", [&device](const char* value) { device = value; } }
    };
    for(int i = 0; i < nopts; i++) {
        auto it = arg_processors.find(opts[i].key);
        if (it == arg_processors.end()) {
            throw runtime_error(string("option '") + opts[i].key + "' does not exist");
        }
        (it->second)(opts[i].value);
    }

    thread th([this, device]() {
        terminate_scan = false;
        scanner = new HCIScanner(true, HCIScanner::FilterDuplicates::Off, HCIScanner::ScanType::Active, device);

        while (!terminate_scan) {
            timeval timeout = { 0, 300000 };
            fd_set fds;
            FD_ZERO(&fds);
            FD_SET(scanner->get_fd(), &fds);

            if (select(scanner->get_fd() + 1, &fds, NULL, NULL, &timeout) < 0 && errno == EINTR) {
                break;
            }
            if (FD_ISSET(scanner->get_fd(), &fds)) {
                try {
                    for (const auto& ad : scanner->get_advertisements()) {
                        auto it = seen_devices.find(ad.address);
                        if (it == seen_devices.end()) {
                            BleatScanPrivateData private_data;
                            seen_devices.emplace(ad.address, private_data);
                            it = seen_devices.find(ad.address);
                        }

                        if (ad.type != LeAdvertisingEventType::SCAN_RSP && ad.local_name) {
                            device_names.emplace(ad.address, ad.local_name->name);

                            it->second.service_uuids.clear();
                            for(const auto& uuid: ad.UUIDs) {
                                it->second.service_uuids.insert(uuid_to_string(uuid));
                            }
                        } else if (ad.type == LeAdvertisingEventType::SCAN_RSP) {
                            it->second.manufacturer_data.clear();

                            if (!ad.manufacturer_specific_data.empty()) {
                                for (const auto& data_it : ad.manufacturer_specific_data) {
                                    BleatScanMftData data = {
                                        data_it.data() + 2,
                                        static_cast<uint32_t>(data_it.size() - 2)
                                    };
                                    it->second.manufacturer_data.emplace(*((uint16_t*) data_it.data()), data);
                                }
                            }

                            if (scan_result_handler != nullptr) {
                                auto name_it = device_names.find(ad.address);
                                BleatScanResult result = {
                                    ad.address.c_str(),
                                    ad.local_name ? ad.local_name->name.c_str() : (name_it == device_names.end() ? "unknown" : name_it->second.c_str()),
                                    (int32_t) ad.rssi,
                                    &it->second
                                };
                                scan_result_handler(scan_result_context, &result);
                            }
                        }
                    }
                } catch (...) {
                    terminate_scan = true;
                }
            }
        }
    });
    swap(scan_thread, th);
}

void BleatScanner_Blepp::stop() {
    terminate_scan = true;
    scan_thread.join();

    delete scanner;
    scanner = nullptr;
    seen_devices.clear();
    device_names.clear();
}

#endif
