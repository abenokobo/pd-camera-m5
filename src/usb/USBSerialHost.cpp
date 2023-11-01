#include "USBSerialHost.h"
#include "common.h"
#include "usb/usb_host.h"



///
USBSerialHost::USBSerialHost()
{
}


///
USBSerialHost::~USBSerialHost()
{
}


///
void USBSerialHost::_usb_lib_task(void* arg)
{
    USBSerialHost* opThis = (USBSerialHost*)arg;
    opThis->usb_lib_task();
}


///
#define USB_SERIAL_HOST_PRIORITY   (20)
void USBSerialHost::usb_lib_task()
{
    while (1)
    {
        // Start handling system events
        uint32_t event_flags;
        usb_host_lib_handle_events(portMAX_DELAY, &event_flags);
        
        if (event_flags & USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS)
        {
            //GLPrintln("USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS");
            usb_host_device_free_all();
        }
        
        if (event_flags & USB_HOST_LIB_EVENT_FLAGS_ALL_FREE)
        {
            // Continue handling USB events to allow device reconnection
            //GLPrintln("USB: All devices freed");
        }
    }
}


///
bool USBSerialHost::Initialize()
{
    const usb_host_config_t host_config =
    {
        .skip_phy_setup = false,
        .intr_flags = ESP_INTR_FLAG_LEVEL1,
    };

    if (usb_host_install(&host_config) != ESP_OK)
    {
        GLPrintln("usb_host_install failed!");
        return false;
    }

    BaseType_t task_created = xTaskCreate(_usb_lib_task, "usb_lib", 4096, this, USB_SERIAL_HOST_PRIORITY, NULL);
    if (task_created != pdTRUE)
    {
        GLPrintln("usb_lib task_create failed!");
        return false;
    }

    return m_acm.Initialize();
}


///
bool USBSerialHost::IsConnected()
{
    return m_acm.IsConnected();
}


///
bool USBSerialHost::Write(uint8_t* buffer, size_t len, uint32_t timeout_ms)
{
    return m_acm.Write(buffer, len, timeout_ms);
}


///
bool USBSerialHost::IsAvailable()
{
    return m_acm.IsAvailable();
}


///
char USBSerialHost::Read()
{
    return m_acm.Read();
}


///
USBSerialHost& USBSerialHost::GetInstance()
{
    static USBSerialHost goSingletone;
    return goSingletone;
}


