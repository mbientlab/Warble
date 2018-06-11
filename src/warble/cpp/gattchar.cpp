/**
 * @copyright MbientLab License
 */

#include "warble/gattchar.h"
#include "gattchar_def.h"

using std::uint8_t;

WarbleGattChar::~WarbleGattChar() {

}

void warble_gattchar_write_async(WarbleGattChar* obj, const uint8_t* value, uint8_t len, void* context, FnVoid_VoidP_WarbleGattCharP_CharP handler) {
    obj->write_async(value, len, context, handler);
}

void warble_gattchar_write_without_resp_async(WarbleGattChar* obj, const uint8_t* value, uint8_t len, void* context, FnVoid_VoidP_WarbleGattCharP_CharP handler) {
    obj->write_without_resp_async(value, len, context, handler);
}

void warble_gattchar_read_async(WarbleGattChar* obj, void* context, FnVoid_VoidP_WarbleGattCharP_UbyteP_Ubyte_CharP handler) {
    obj->read_async(context, handler);
}

void warble_gattchar_enable_notifications_async(WarbleGattChar* obj, void* context, FnVoid_VoidP_WarbleGattCharP_CharP handler) {
    obj->enable_notifications_async(context, handler);
}

void warble_gattchar_disable_notifications_async(WarbleGattChar* obj, void* context, FnVoid_VoidP_WarbleGattCharP_CharP handler) {
    obj->disable_notifications_async(context, handler);
}

void warble_gattchar_on_notification_received(WarbleGattChar* obj, void* context, FnVoid_VoidP_WarbleGattCharP_UbyteP_Ubyte handler) {
    obj->on_notification_received(context, handler);
}

const char* warble_gattchar_get_uuid(const WarbleGattChar* obj) {
    return obj->get_uuid();
}

WarbleGatt* warble_gattchar_get_gatt(const WarbleGattChar* obj) {
    return obj->get_gatt();
}