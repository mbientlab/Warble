/**
 * @copyright MbientLab License
 */
#pragma once

#ifdef API_BLEPP

#include "gatt_def.h"
#include "gattchar_def.h"

#include "blepp/blestatemachine.h"

#include <condition_variable>
#include <mutex>
#include <thread>
#include <unordered_map>

struct BleatGattChar_Blepp;

struct BleatGatt_Blepp : public BleatGatt {
    BleatGatt_Blepp(int nopts, const BleatGattOption* opts);
    virtual ~BleatGatt_Blepp();

    virtual void connect_async(void* context, Void_VoidP_BleatGattP_Uint handler);
    virtual void disconnect();
    virtual void on_disconnect(void* context, Void_VoidP_BleatGattP_Uint handler);

    virtual BleatGattChar* find_characteristic(const std::string& uuid);

private:
    friend BleatGattChar_Blepp;
    
    const char *mac, *device;

    void *connect_context, *on_disconnect_context;
    Void_VoidP_BleatGattP_Uint connect_handler, on_disconnect_handler;

    BleatGattChar_Blepp* active_char;
    void* write_context;
    Void_VoidP_BleatGattCharP write_handler;

    BLEPP::BLEGATTStateMachine gatt;
    std::unordered_map<std::string, BleatGattChar_Blepp*> characteristics;

    std::condition_variable state_machine_cv;
    std::mutex cv_m;
    std::unique_lock<std::mutex> cv_lock;
    std::thread blepp_state_machine;
    bool terminate_state_machine;
};

struct BleatGattChar_Blepp : public BleatGattChar {
    BleatGattChar_Blepp(BleatGatt_Blepp* owner, BLEPP::Characteristic& ble_char);

    virtual ~BleatGattChar_Blepp();

    virtual void write_async(const std::uint8_t* value, std::uint8_t len, void* context, Void_VoidP_BleatGattCharP handler);
    virtual void write_without_resp_async(const std::uint8_t* value, std::uint8_t len, void* context, Void_VoidP_BleatGattCharP handler);

    virtual void read_async(void* context, Void_VoidP_BleatGattCharP_UbyteC_Ubyte handler);

    virtual void enable_notifications_async(void* context, Void_VoidP_BleatGattCharP handler);
    virtual void disable_notifications_async(void* context, Void_VoidP_BleatGattCharP handler) ;
    virtual void set_value_changed_handler(void* context, Void_VoidP_BleatGattCharP_UbyteC_Ubyte handler);

private:
    BleatGatt_Blepp* owner;
    BLEPP::Characteristic& ble_char;

    void *read_context, *value_changed_context;
    Void_VoidP_BleatGattCharP_UbyteC_Ubyte read_handler, value_changed_handler;
};

#endif
