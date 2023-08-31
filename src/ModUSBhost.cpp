#include "ModUSBhost.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "M5Unified.h"



///
void ModUSBhost::_client_event_callback(const usb_host_client_event_msg_t *event_msg, void *arg)
{
    ModUSBhost *host = (ModUSBhost *)arg;
    if (event_msg->event == USB_HOST_CLIENT_EVENT_NEW_DEV)
    {
        if (host->_client_event_cb)
        {
            host->_client_event_cb(event_msg, arg);
        } else {
            host->open(event_msg);
        }
    }
}


///
void ModUSBhost::client_async_seq_task(void *param)
{
    auto opThis = (ModUSBhost*)param;
    if (opThis == NULL)
    {
        return;
    }

    usb_host_client_handle_t client_hdl = opThis->client_hdl;
    uint32_t event_flags;
    while (1)
    {
        usb_host_client_handle_events(client_hdl, 1);

        if (ESP_OK == usb_host_lib_handle_events(1, &event_flags))
        {
            if (event_flags & USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS)
            {
                usb_host_device_free_all();
            }
            if (event_flags & USB_HOST_LIB_EVENT_FLAGS_ALL_FREE)
            {
                break;
            }
        }

        if (opThis->m_usb_host_task_cb)
        {
            (*opThis->m_usb_host_task_cb)(opThis->m_usb_host_task_param);
        }

        delay(10);
    }
    usb_host_client_deregister(client_hdl);
    vTaskDelete(NULL);
}


///
ModUSBhost::ModUSBhost()
: m_usb_host_task_cb(NULL)
, m_usb_host_task_param(NULL)
{
}


///
ModUSBhost::~ModUSBhost()
{
}


///
bool ModUSBhost::ModUSBhostInit()
{
    const usb_host_config_t config = {
        .intr_flags = ESP_INTR_FLAG_LEVEL1,
    };
    esp_err_t err = usb_host_install(&config);
    if (err != ESP_OK)
    {
        M5.Display.print("usb_host_install failed.");
        return false;
    }

    const usb_host_client_config_t client_config = {
        .is_synchronous = false,
        .max_num_event_msg = 5,
        .async = {
            .client_event_callback = ModUSBhost::_client_event_callback,
            .callback_arg = this
        }
    };

    err = usb_host_client_register(&client_config, &client_hdl);
    if (err != ESP_OK)
    {
        M5.Display.print("usb_host_client_register failed.");
        return false;
    }

    xTaskCreatePinnedToCore(&ModUSBhost::client_async_seq_task, "async", 4 * 512, this, 1, NULL, APP_CPU_NUM);
    delay(100);

    return true;
}


///
void ModUSBhost::registerUsbHostTaskCb(UsbHostTaskCallback cb, void* param)
{
    m_usb_host_task_cb = cb;
    m_usb_host_task_param = param;
}

