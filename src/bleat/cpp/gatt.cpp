#include "bleat/gatt.h"
#include "gatt_def.h"

#include "api_blepp.h"

BleatGatt::~BleatGatt() {

}

BleatGatt* bleat_gatt_new(const char* mac) {
    return new BleatGatt_Blepp(mac);
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
