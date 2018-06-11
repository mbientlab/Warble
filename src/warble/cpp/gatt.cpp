/**
 * @copyright MbientLab License
 */

#include "warble/gatt.h"
#include "gatt_def.h"

using std::int32_t;

WarbleGatt::~WarbleGatt() {

}

WarbleGatt* warble_gatt_create(const char* mac) {
    WarbleOption opts[] = {
        {"mac", mac}
    };
    return warble_gatt_create_with_options(1, opts);
}

WarbleGatt* warble_gatt_create_with_options(int32_t nopts, const WarbleOption* opts) {
    return warblegatt_create(nopts, opts);
}

void warble_gatt_delete(WarbleGatt* obj) {
    delete obj;
}

void warble_gatt_connect_async(WarbleGatt* obj, void* context, FnVoid_VoidP_WarbleGattP_CharP handler) {
    obj->connect_async(context, handler);
}

void warble_gatt_disconnect(WarbleGatt* obj) {
    obj->disconnect();
}

void warble_gatt_on_disconnect(WarbleGatt* obj, void* context, FnVoid_VoidP_WarbleGattP_Int handler) {
    obj->on_disconnect(context, handler);
}

int32_t warble_gatt_is_connected(const WarbleGatt* obj) {
    return obj->is_connected();
}

WarbleGattChar* warble_gatt_find_characteristic(const WarbleGatt* obj, const char* uuid) {
    return obj->find_characteristic(uuid);
}

int32_t warble_gatt_has_service(const WarbleGatt* obj, const char* uuid) {
    return obj->service_exists(uuid);
}