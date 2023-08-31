#include "USBSerialHost.h"
#include <M5Unified.h>




///
void USBSerialHost::acm_events(int event, void* data, size_t len)
{
    switch (event)
    {
    case CDC_CTRL_SET_CONTROL_LINE_STATE:
        m_opAcmDevice->setLineCoding(115200, 0, 0, 8);
        break;

    case CDC_DATA_IN:
    {
        m_opAcmDevice->INDATA();
        if (len > 0)
        {
            auto buffer = new DataBuffer(data, len);
            xQueueSendToBack(m_queue, &buffer, portMAX_DELAY);
        }
        break;
    }
    case CDC_DATA_OUT:
        break;

    case CDC_CTRL_SET_LINE_CODING:
        break;
    }
}


///
void USBSerialHost::_acm_events(int event, void* data, size_t len)
{
    GetInstance().acm_events(event, data, len);
}


///
void USBSerialHost::client_event_callback(const usb_host_client_event_msg_t* event_msg, void *arg)
{
    if (event_msg->event == USB_HOST_CLIENT_EVENT_NEW_DEV)
    {
        m_oHost.open(event_msg);
        usb_device_info_t info = m_oHost.getDeviceInfo();
        const usb_device_desc_t *dev_desc = m_oHost.getDeviceDescriptor();
        int offset = 0;
        for (size_t i = 0; i < dev_desc->bNumConfigurations; i++)
        {
            const usb_config_desc_t *config_desc = m_oHost.getConfigurationDescriptor();
            for (size_t n = 0; n < config_desc->bNumInterfaces; n++)
            {
                const usb_intf_desc_t *intf = usb_parse_interface_descriptor(config_desc, n, 0, &offset);
                if (intf->bInterfaceClass == 0x0a) // CDC ACM
                {
                    m_opAcmDevice = new ModUSBacmDevice(config_desc, intf, n, &m_oHost);
                    n = config_desc->bNumInterfaces;
                    if (m_opAcmDevice)
                    {
                        m_opAcmDevice->onEvent(_acm_events);
                        m_opAcmDevice->init();
                        m_opAcmDevice->setControlLine(1, 1);
                        m_opAcmDevice->INDATA();
                    }
                }
            }
        }
    }
}


///
void USBSerialHost::_client_event_callback(const usb_host_client_event_msg_t* event_msg, void *arg)
{
    GetInstance().client_event_callback(event_msg, arg);
}


///
USBSerialHost::USBSerialHost()
: m_queue(NULL)
, m_dataActive(NULL)
, m_opAcmDevice(NULL)
{
}


///
USBSerialHost::~USBSerialHost()
{
    if (m_queue)
    {
        vQueueDelete(m_queue);
    }
}



bool USBSerialHost::Initialize()
{
    m_queue = xQueueCreate(10, sizeof(DataBuffer*));
    m_oHost.registerClientCb(USBSerialHost::_client_event_callback);
    m_oHost.registerUsbHostTaskCb(USBSerialHost::_USBHostTask, this);
    return m_oHost.ModUSBhostInit();
}


///
bool USBSerialHost::IsConnected()
{
    if (m_opAcmDevice)
    {
        return m_opAcmDevice->isConnected();
    }
    return false;
}


///
SemaphoreHandle_t USBSerialHost::Write(uint8_t* buffer, size_t len)
{
    if (!m_opAcmDevice)
    {
        return NULL;
    }
    return m_opAcmDevice->OutDataAsync(buffer, len);
}


///
bool USBSerialHost::IsAvailable()
{
    if (m_dataActive)
    {
        if (m_dataActive->IsAvailable())
        {
            return true;
        }

        delete m_dataActive;
        m_dataActive = NULL;
    }

    xQueueReceive(m_queue, &m_dataActive, 1);
    if (m_dataActive)
    {
        return m_dataActive->IsAvailable();
    }

    return false;
}


///
char USBSerialHost::Read()
{
    if (!m_dataActive)
    {
        assert(false);
        return '\0';
    }

    char ret;
    if (!m_dataActive->Read(ret))
    {
        assert(false);
        return '\0';
    }

    return ret;
}


///
void USBSerialHost::USBHostTask()
{
    if (m_opAcmDevice)
    {
        m_opAcmDevice->DoOutData();
    }
}


///
void USBSerialHost::_USBHostTask(void* param)
{
    ((USBSerialHost*)param)->USBHostTask();
}


///
USBSerialHost& USBSerialHost::GetInstance()
{
    static USBSerialHost goSingletone;
    return goSingletone;
}




