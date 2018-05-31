/**
 * @copyright MbientLab License
 */

#include "bleat/gatt.h"
#include "gatt_def.h"

using std::int32_t;

BleatGatt::~BleatGatt() {

}

BleatGatt* bleat_gatt_create(const char* mac) {
    BleatOption opts[] = {
        {"mac", mac}
    };
    return bleat_gatt_create_with_options(1, opts);
}

BleatGatt* bleat_gatt_create_with_options(int32_t nopts, const BleatOption* opts) {
    return bleatgatt_create(nopts, opts);
}

void bleat_gatt_delete(BleatGatt* obj) {
    delete obj;
}

void bleat_gatt_connect_async(BleatGatt* obj, void* context, Void_VoidP_BleatGattP_CharP handler) {
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

int32_t bleat_gatt_has_service(BleatGatt* obj, const char* uuid) {
    return obj->service_exists(uuid);
}