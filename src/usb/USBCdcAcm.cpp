#include "USBCdcAcm.h"


#define USB_CDC_ACM_HOST_TASK_PRIORITY   (20)
#define USB_CDC_ACM_DATA_QUEUE_COUNT    (10)

#define PLAYDATE_USB_DEVICE_VID      (0x1331)
#define PLAYDATE_USB_DEVICE_PID      (0x5740)


#define USB_CDC_ACM_OUT_BUFFER_SIZE (512)
#define USB_CDC_ACM_IN_BUFFER_SIZE (512)


///
void USBCdcAcm::_cdc_acm_host_dev_callback(const cdc_acm_host_dev_event_data_t* event, void* user_ctx)
{
    USBCdcAcm* opThis = (USBCdcAcm*)user_ctx;
    opThis->cdc_acm_host_dev_callback(event);
}


///
bool USBCdcAcm::_cdc_acm_data_callback(const uint8_t* data, size_t data_len, void* arg)
{
    USBCdcAcm* opThis = (USBCdcAcm*)arg;
    return opThis->cdc_acm_data_callback(data, data_len);
}


///
void USBCdcAcm::cdc_acm_host_dev_callback(const cdc_acm_host_dev_event_data_t* event)
{
    switch (event->type) {
    case CDC_ACM_HOST_ERROR:
        GLPrintf("CDC-ACM error has occurred, err_no = %i\n", event->data.error);
        break;
    
    case CDC_ACM_HOST_DEVICE_DISCONNECTED:
        GLPrintln("Playdate disconnected!");
        xSemaphoreGive(m_smpConnect);
        break;

    case CDC_ACM_HOST_SERIAL_STATE:
        GLPrintf("Serial state notif 0x%04X\n", event->data.serial_state.val);
        break;
    
    case CDC_ACM_HOST_NETWORK_CONNECTION:
    default:
        GLPrintf("Unsupported CDC event: %i\n", event->type);
        break;
    }
}


///
bool USBCdcAcm::cdc_acm_data_callback(const uint8_t* data, size_t data_len)
{
    if (data_len > 0)
    {
        auto buffer = new DataBuffer((void*)data, data_len);
        xQueueSendToBack(m_queData, &buffer, portMAX_DELAY);

        //GLPrintf("R: ");
        //GLWrite(data, data_len);
    }
    return true;
}


///
void USBCdcAcm::_cdc_acm_task(void* arg)
{
    USBCdcAcm* opThis = (USBCdcAcm*)arg;
    opThis->cdc_acm_task();
}


///
void USBCdcAcm::cdc_acm_task()
{
    const cdc_acm_host_device_config_t dev_config = {
        .connection_timeout_ms = 1000,
        .out_buffer_size = USB_CDC_ACM_OUT_BUFFER_SIZE,
        .in_buffer_size = USB_CDC_ACM_IN_BUFFER_SIZE,
        .event_cb = _cdc_acm_host_dev_callback,
        .data_cb = _cdc_acm_data_callback,
        .user_arg = this
    };

    while (true)
    {
        assert(m_hCDC == NULL);
        esp_err_t err = cdc_acm_host_open(PLAYDATE_USB_DEVICE_VID, PLAYDATE_USB_DEVICE_PID, 0, &dev_config, &m_hCDC);
        if (ESP_OK != err) {
            continue;
        }

        if (m_hCDC == NULL)
        {
            assert(false);
            continue;
        }

        GLPrintln("Playdate connected!");
        //vTaskDelay(pdMS_TO_TICKS(100));

        xSemaphoreTake(m_smpConnect, portMAX_DELAY);
        if (cdc_acm_host_close(m_hCDC) != ESP_OK)
        {
            GLPrintln("cdc_acm_host_close failed!");
        }

        m_hCDC = NULL;

        RemoveQueueData();
    }
}


///
void USBCdcAcm::RemoveQueueData()
{
    if (m_dataActive)
    {
        delete m_dataActive;
        m_dataActive = NULL;
    }
    
    while (xQueueReceive(m_queData, &m_dataActive, 10) && m_dataActive)
    {
        delete m_dataActive;
        m_dataActive = NULL;
    }
}


///
void USBCdcAcm::Finalize()
{
    if (m_smpConnect)
    {
        xSemaphoreGive(m_smpConnect);
        vTaskDelay(pdMS_TO_TICKS(100));

        vSemaphoreDelete(m_smpConnect);
        m_smpConnect = NULL;
    }

    if (m_hCDC)
    {
        cdc_acm_host_close(m_hCDC);
        m_hCDC = NULL;
    }

    RemoveQueueData();

    if (m_queData)
    {
        vQueueDelete(m_queData);
    }

    cdc_acm_host_uninstall();
}



///
USBCdcAcm::USBCdcAcm()
: m_hCDC(NULL)
, m_smpConnect(NULL)
, m_queData(NULL)
, m_dataActive(NULL)
{
}


///
USBCdcAcm::~USBCdcAcm()
{
    Finalize();
}


///
bool USBCdcAcm::Initialize()
{
    do {
        if (cdc_acm_host_install(NULL) != ESP_OK)
        {
            GLPrintln("cdc_acm_host_install failed!");
            break;
        }

        m_smpConnect = xSemaphoreCreateBinary();
        if (!m_smpConnect)
        {
            GLPrintln("xSemaphoreCreateBinary failed!");
            break;
        }

        m_queData = xQueueCreate(USB_CDC_ACM_DATA_QUEUE_COUNT, sizeof(DataBuffer*));
        if (!m_queData)
        {
            GLPrintln("xSemaphoreCreateBinary failed!");
            break;
        }

        BaseType_t task_created = xTaskCreate(_cdc_acm_task, "cdc_acm_task", 4096, this, USB_CDC_ACM_HOST_TASK_PRIORITY, NULL);
        if (task_created != pdTRUE)
        {
            GLPrintln("cdc_acm_task create err!");
            break;
        }

        return true;

    } while (false);
    
    Finalize();

    return false;
}


///
bool USBCdcAcm::IsConnected()
{
    if (m_hCDC)
    {
        return true;
    }
    return false;
}


///
bool USBCdcAcm::Write(uint8_t* buffer, size_t len, uint32_t timeout_ms)
{
    if (!m_hCDC)
    {
        return false;
    }

    int remain = len;
    int out;

    while (remain > 0)
    {
        out = remain;
        if (out > USB_CDC_ACM_OUT_BUFFER_SIZE)
        {
            out = USB_CDC_ACM_OUT_BUFFER_SIZE;
        }

        //GLPrintf("W: size(%d)\n", out);
        //GLWrite(buffer, out);

        if (esp_err_t e = cdc_acm_host_data_tx_blocking(m_hCDC, buffer, out, timeout_ms) != ESP_OK)
        {
            GLPrintf("cdc_acm_host_data_tx_blocking faild! %s\n", esp_err_to_name(e));
            return false;
        }
        remain -= out;
        buffer += out;
    }

    return true;
}


///
bool USBCdcAcm::IsAvailable()
{
    if (!m_hCDC)
    {
        return false;
    }

    if (m_dataActive)
    {
        if (m_dataActive->IsAvailable())
        {
            return true;
        }

        delete m_dataActive;
        m_dataActive = NULL;
    }

    xQueueReceive(m_queData, &m_dataActive, 1);
    if (m_dataActive)
    {
        return m_dataActive->IsAvailable();
    }

    return false;
}


///
char USBCdcAcm::Read()
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
