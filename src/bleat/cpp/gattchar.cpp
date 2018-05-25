/**
 * @copyright MbientLab License
 */

#include "bleat/gattchar.h"
#include "gattchar_def.h"

using std::uint8_t;

BleatGattChar::~BleatGattChar() {

}

void bleat_gattchar_write_async(BleatGattChar* obj, const uint8_t* value, uint8_t len, void* context, Void_VoidP_BleatGattCharP_CharP handler) {
    obj->write_async(value, len, context, handler);
}

void bleat_gattchar_write_without_resp_async(BleatGattChar* obj, const uint8_t* value, uint8_t len, void* context, Void_VoidP_BleatGattCharP_CharP handler) {
    obj->write_without_resp_async(value, len, context, handler);
}

void bleat_gattchar_read_async(BleatGattChar* obj, void* context, Void_VoidP_BleatGattCharP_UbyteC_Ubyte_CharP handler) {
    obj->read_async(context, handler);
}

void bleat_gattchar_enable_notifications_async(BleatGattChar* obj, void* context, Void_VoidP_BleatGattCharP_CharP handler) {
    obj->enable_notifications_async(context, handler);
}

void bleat_gattchar_disable_notifications_async(BleatGattChar* obj, void* context, Void_VoidP_BleatGattCharP_CharP handler) {
    obj->disable_notifications_async(context, handler);
}

void bleat_gattchar_set_value_changed_handler(BleatGattChar* obj, void* context, Void_VoidP_BleatGattCharP_UbyteC_Ubyte handler) {
    obj->set_value_changed_handler(context, handler);
}
