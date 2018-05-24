#include "blepp_scanner.h"

#include <cstdint>
#include <unistd.h>

using namespace std;
/**
 * @copyright MbientLab License
 */
using namespace BLEPP;

BleatScanner_Blepp::BleatScanner_Blepp() : scanner(false, HCIScanner::FilterDuplicates::Off, HCIScanner::ScanType::Active), cv_lock(cv_m) {
    thread th([this]() {        
        while(true) {
            scan_cv.wait(cv_lock);

            while(!terminate_scan) {
                timeval timeout = { 0, 300000 };
                fd_set fds;
                FD_ZERO(&fds);
                FD_SET(scanner.get_fd(), &fds);

                if (select(scanner.get_fd() + 1, &fds, NULL, NULL,  &timeout) < 0 && errno == EINTR) {
                    break;
                }
                if(FD_ISSET(scanner.get_fd(), &fds)) {
                    try {
                        for(const auto& ad: scanner.get_advertisements()) {
                            auto it = seen_devices.find(ad.address);
                            if (it == seen_devices.end()) {
                                if (ad.type != LeAdvertisingEventType::SCAN_RSP && ad.local_name) {
                                    seen_devices.emplace(ad.address, ad.local_name->name);
                                }
                            } else {
                                if (ad.type == LeAdvertisingEventType::SCAN_RSP) {
                                    BleatScanMftData *manufacturer_data;
                                    int i = 0;

                                    if (!ad.manufacturer_specific_data.empty()) {
                                        manufacturer_data = new BleatScanMftData[ad.manufacturer_specific_data.size()];

                                        for(const auto& it: ad.manufacturer_specific_data) {
                                            manufacturer_data[i].value = it.data() + 2;
                                            manufacturer_data[i].value_size = it.size() - 2;
                                            manufacturer_data[i].company_id = *((const uint16_t*) it.data());
                                            i++;
                                        }
                                    } else {
                                        manufacturer_data = nullptr;
                                    }

                                    BleatScanResult result = {
                                        it->first.c_str(),
                                        it->second.c_str(),
                                        manufacturer_data,
                                        i,
                                        (int32_t) ad.rssi,
                                    };
                                    scan_result_handler(scan_result_context, &result);

                                    delete[] manufacturer_data;
                                }
                            }
                        }
                    } catch (...) {
                        terminate_scan = true;
                    }
                }
            }
        }
    });
    swap(scan_thread, th);
}

BleatScanner_Blepp::~BleatScanner_Blepp() {

}

void BleatScanner_Blepp::set_handler(void* context, Void_VoidP_BleatScanResultP handler) {
    scan_result_context = context;
    scan_result_handler = handler;
}

void BleatScanner_Blepp::start() {
    scanner.start();
    terminate_scan = false;
    scan_cv.notify_all();
}

void BleatScanner_Blepp::stop() {
    scanner.stop();
    terminate_scan = true;
}