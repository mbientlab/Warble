/**
 * @copyright MbientLab License
 */

#include "bleat/gatt.h"
#include "gatt_def.h"

#ifdef API_BLEPP
#include "api_blepp.h"
#define BLEAT_GATT_IMPL BleatGatt_Blepp
#elif API_WIN10
#include "win10_api.h"
#define BLEAT_GATT_IMPL BleatGatt_Win10
#endif

BleatGatt::~BleatGatt() {

}

BleatGatt* bleat_gatt_new(const char* mac) {
    BleatGattOption opts[] = {
        {"mac", mac}
    };
    return bleat_gatt_new_with_config(1, opts);
}

BleatGatt* bleat_gatt_new_with_config(BLEAT_INT nopts, const BleatGattOption* opts) {
    return new BLEAT_GATT_IMPL(nopts, opts);
}

void bleat_gatt_free(BleatGatt* obj) {
    delete obj;
}

void bleat_gatt_connect_async(BleatGatt* obj, void* context, Void_VoidP_BleatGattP_Uint handler) {
    obj->connect_async(context, handler);
}

void bleat_gatt_disconnect(BleatGatt* obj) {
    obj->disconnect();
}

void bleat_gatt_on_disconnect(BleatGatt* obj, void* context, Void_VoidP_BleatGattP_Uint handler) {
    obj->on_disconnect(context, handler);
}

BleatGattChar* bleat_gatt_find_characteristic(BleatGatt* obj, const char* uuid) {
    return obj->find_characteristic(uuid);
}