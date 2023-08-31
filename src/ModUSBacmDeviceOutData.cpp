#include "ModUSBacmDeviceOutData.h"
#include "M5Unified.h"



///
ModUSBacmDeviceOutData::ModUSBacmDeviceOutData()
: m_xfer_write(NULL)
, m_OutData(NULL)
, m_OutRemain(0)
, m_semaphore(NULL)
, m_bCom(false)
{
}


///
ModUSBacmDeviceOutData::~ModUSBacmDeviceOutData()
{
    if (m_semaphore)
    {
        vSemaphoreDelete(m_semaphore);
    }
}


///
SemaphoreHandle_t ModUSBacmDeviceOutData::Initialize(usb_transfer_t* write, uint8_t* data, size_t len)
{
    if ((write == NULL)
    || (data == NULL)
    || (len <= 0))
    {
        return NULL;
    }

    m_semaphore = xSemaphoreCreateBinary();
    if (!m_semaphore)
    {
        return NULL;
    }

    m_xfer_write = write;
    m_OutData = data;
    m_OutRemain = len;
    return m_semaphore;
}


///
void ModUSBacmDeviceOutData::OnOutDataCB()
{
    m_bCom = false;
        
    if (!HasRemainDataAsync())
    {
        if (m_semaphore)
        {
            xSemaphoreGive(m_semaphore);
            m_semaphore = NULL;
        }
    }
}


///
bool ModUSBacmDeviceOutData::HasRemainDataAsync()
{
    if (!m_OutData || (m_OutRemain <= 0))
    {
        return false;
    }
    return true;
}


///
bool ModUSBacmDeviceOutData::DoOutData()
{
    if (m_semaphore == NULL)
    {
        // finished
        return true;
    }

    if (m_bCom)
    {
        // wait callback
        return false;
    }

    if (!HasRemainDataAsync())
    {
        // wait callback
        return false;
    }

    size_t send;
    if (m_OutRemain > TRANSFER_ALLOC_SIZE_WRITE)
    {
        send = TRANSFER_ALLOC_SIZE_WRITE;
    }
    else
    {
        send = m_OutRemain;
    }

    m_xfer_write->num_bytes = send;
    ::memcpy(m_xfer_write->data_buffer, m_OutData, send);
    esp_err_t err = usb_host_transfer_submit(m_xfer_write);
    if (err)
    {
        // error finished.
        return true;
    }

    m_OutData += send;
    m_OutRemain -= send;
    m_bCom = true;

    return false;
}



