#ifndef USB_LIB_H
#define USB_LIB_H
#include "usb/usb_host.h"
#include "usb/hid_usage_mouse.h"
#include "usb/hid_host.h"
#include "usb/hid.h"
#include "esp_log.h"

#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "freertos/FreeRTOS.h"

#include "driver/gpio.h"

typedef enum
{
    APP_EVENT = 0,
    APP_EVENT_HID_HOST
} app_event_group_t;

typedef struct
{
    app_event_group_t event_group;
    /* HID Host - Device related info */
    struct
    {
        hid_host_device_handle_t handle;
        hid_host_driver_event_t event;
        void *arg;
    } hid_host_device;
} app_event_queue_t;

typedef struct
{
    enum key_state
    {
        KEY_STATE_PRESSED = 0x00,
        KEY_STATE_RELEASED = 0x01
    } state;
    uint8_t modifier;
    uint8_t key_code;
} key_event_t;

static const char *hid_proto_name_str[] = {
    "NONE",
    "KEYBOARD",
    "MOUSE"};

class USB
{
    static QueueHandle_t app_event_queue;
    static const char *TAG ;
    BaseType_t task_created;
    app_event_queue_t evt_queue;
    USB();
    ~USB();
private:
    static void hid_print_new_device_report_header(hid_protocol_t proto);
    static void hid_host_mouse_report_callback(const uint8_t *const data, const int length);
    static void hid_host_device_callback(hid_host_device_handle_t hid_device_handle,
                                  const hid_host_driver_event_t event,
                                  void *arg);
    static void usb_lib_task(void *arg);
    void hid_host_device_event(hid_host_device_handle_t hid_device_handle,
                               const hid_host_driver_event_t event,
                               void *arg);
    static void hid_host_interface_callback(hid_host_device_handle_t hid_device_handle,
                                     const hid_host_interface_event_t event,
                                     void *arg);
    static void hid_host_generic_report_callback(const uint8_t *const data, const int length);
};
#endif