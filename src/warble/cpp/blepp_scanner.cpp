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
#include <locale>
#include <sstream>
#include <string>
#include <thread>
#include <unistd.h>
#include <unordered_map>

using namespace std;
using namespace BLEPP;

static inline string to_upper(const string& in) {
    locale loc;
    stringstream buffer;

    for(auto s: in) {
        buffer << toupper(s, loc);
    }
    return buffer.str();
}

class WarbleScanner_Blepp : public WarbleScanner {
public:
    WarbleScanner_Blepp();
    virtual ~WarbleScanner_Blepp();

    virtual void set_handler(void* context, FnVoid_VoidP_WarbleScanResultP handler);
    virtual void start(WARBLE_INT nopts, const WarbleOption* opts);
    virtual void stop();

private:
    BLEPP::HCIScanner* scanner;

    void* scan_result_context;
    FnVoid_VoidP_WarbleScanResultP scan_result_handler;
    std::thread scan_thread;
    bool terminate_scan;
};

WarbleScanner* warble_scanner_create() {
    return new WarbleScanner_Blepp();
}

WarbleScanner_Blepp::WarbleScanner_Blepp() : scanner(nullptr), scan_result_context(nullptr), scan_result_handler(nullptr) {
}

WarbleScanner_Blepp::~WarbleScanner_Blepp() {
    stop();
}

void WarbleScanner_Blepp::set_handler(void* context, FnVoid_VoidP_WarbleScanResultP handler) {
    scan_result_context = context;
    scan_result_handler = handler;
}

void WarbleScanner_Blepp::start(int32_t nopts, const WarbleOption* opts) {
    if (scanner != nullptr) {
        return;
    }

    const char *device = "";
    auto scanType = HCIScanner::ScanType::Active;
    unordered_map<string, function<void(const char*)>> arg_processors = {
        { "hci", [&device](const char* value) { device = value; } },
        { "scan-type", [&scanType](const char* value) {
            if (!strcmp(value, "passive")) {
                scanType = HCIScanner::ScanType::Passive;
            } else if (strcmp(value, "active")) {
                throw runtime_error("invalid value for \'scan-type\' option (blepp api): one of [active, passive]");
            }
        }}
    };
    for(int i = 0; i < nopts; i++) {
        auto it = arg_processors.find(opts[i].key);
        if (it == arg_processors.end()) {
            throw runtime_error(string("invalid ble scan option '") + opts[i].key + "'");
        }
        (it->second)(opts[i].value);
    }

    thread th([this, device, scanType]() {
        unordered_map<string, WarbleScanPrivateData> seen_devices;
        unordered_map<string, string> device_names;

        terminate_scan = false;
        scanner = new HCIScanner(true, HCIScanner::FilterDuplicates::Off, scanType, device);

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
                        auto addr = to_upper(ad.address);
                        auto it = seen_devices.find(addr);
                        if (it == seen_devices.end()) {
                            WarbleScanPrivateData private_data;
                            seen_devices.emplace(addr, private_data);
                            it = seen_devices.find(addr);
                        }

                        if (ad.type != LeAdvertisingEventType::SCAN_RSP && ad.local_name) {
                            if (scanType == HCIScanner::ScanType::Passive) {
                                WarbleScanPrivateData private_data;
                                for(const auto& uuid: ad.UUIDs) {
                                    private_data.service_uuids.insert(uuid_to_string(uuid));
                                }

                                WarbleScanResult result = {
                                    addr.c_str(),
                                    ad.local_name ? ad.local_name->name.c_str() : "",
                                    (int32_t) ad.rssi,
                                    &private_data
                                };
                                scan_result_handler(scan_result_context, &result);
                            } else {
                                device_names.emplace(addr, ad.local_name->name);

                                it->second.service_uuids.clear();
                                for(const auto& uuid: ad.UUIDs) {
                                    it->second.service_uuids.insert(uuid_to_string(uuid));
                                }
                            }
                        } else if (ad.type == LeAdvertisingEventType::SCAN_RSP) {
                            it->second.manufacturer_data.clear();

                            if (!ad.manufacturer_specific_data.empty()) {
                                for (const auto& data_it : ad.manufacturer_specific_data) {
                                    WarbleScanMftData data = {
                                        data_it.data() + 2,
                                        static_cast<uint32_t>(data_it.size() - 2)
                                    };
                                    it->second.manufacturer_data.emplace(*((uint16_t*) data_it.data()), data);
                                }
                            }

                            if (scan_result_handler != nullptr) {
                                auto name_it = device_names.find(addr);
                                WarbleScanResult result = {
                                    addr.c_str(),
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

        delete scanner;
        scanner = nullptr;

        seen_devices.clear();
        device_names.clear();
    });
    swap(scan_thread, th);
}

void WarbleScanner_Blepp::stop() {
    terminate_scan = true;
    scan_thread.join();
}

#endif
