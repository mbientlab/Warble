/**
 * @copyright MbientLab License
 */
#ifdef API_BLEPP

#include "blepp_utils.h"
#include "blepp/pretty_printers.h"

using std::string;
using namespace BLEPP;

string uuid_to_string(const UUID& uuid) {
    char buffer[37];
    switch(uuid.type) {
    case BT_UUID16:
        sprintf(buffer, "0000%.4x-0000-1000-8000-00805f9b34fb", uuid.value.u16);
        return buffer;
    case BT_UUID32:
        sprintf(buffer, "%.8x-0000-1000-8000-00805f9b34fb", uuid.value.u32);
        return buffer;
    default:
        return to_str(uuid);
    }   
}

#endif