/**
 * @copyright MbientLab License
 */

#include "warble/lib.h"
#include "lib_def.h"

#ifdef API_BLEPP
#include "blepp/blestatemachine.h"

#include <functional>
#include <stdexcept>
#include <string>
#include <unordered_map>

using namespace BLEPP;
#endif

using namespace std;

#ifdef API_WIN10
#include <wrl/wrappers/corewrappers.h>
static Microsoft::WRL::Wrappers::RoInitializeWrapper initialize(RO_INIT_MULTITHREADED);
#endif

const char* warble_lib_version() {
    return WARBLE_LIB_VERSION;
}

const char* warble_lib_config() {
    return WARBLE_LIB_CONFIG;
}

void warble_lib_init(int32_t nopts, const WarbleOption* opts) {
#ifdef API_BLEPP
    unordered_map<string, function<void(const char*)>> arg_processors = {
        {"log-level", [](const char* value) {
            if (!strcmp(value, "error")) {
                log_level = Error;
            } else if (!strcmp(value, "warning")) {
                log_level = Warning;
            } else if (!strcmp(value, "info")) {
                log_level = Info;
            } else if (!strcmp(value, "debug")) {
                log_level = Debug;
            } else if (!strcmp(value, "trace")) {
                log_level = Trace;
            }
        }}
    };

    for (int i = 0; i < nopts; i++) {
        auto it = arg_processors.find(opts[i].key);
        if (it == arg_processors.end()) {
            throw runtime_error(string("option '") + opts[i].key + "' does not exist");
        }
        (it->second)(opts[i].value);
    }
#endif
}