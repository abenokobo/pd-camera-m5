#pragma once
#include "usb_host.hpp"


typedef void (*UsbHostTaskCallback)(void* param);

class ModUSBhost : public USBhost
{
protected:


    ///
    UsbHostTaskCallback m_usb_host_task_cb;

    ///
    void* m_usb_host_task_param;

    ///
    static void _client_event_callback(const usb_host_client_event_msg_t *event_msg, void *arg);

    ///
    static void client_async_seq_task(void *param);



public:


    ///
    ModUSBhost();

    ///
    ~ModUSBhost();

    ///
    bool ModUSBhostInit();

    ///
    void registerUsbHostTaskCb(UsbHostTaskCallback cb, void* param);
};

