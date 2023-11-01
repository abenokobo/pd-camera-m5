#pragma once
#include "common.h"
#include "usb/cdc_acm_host.h"
#include "DataBuffer.h"
#include <semphr.h>



class USBCdcAcm
{
protected:


    ///
    cdc_acm_dev_hdl_t m_hCDC;

    ///
    SemaphoreHandle_t m_smpConnect;

    ///
    QueueHandle_t m_queData;    

    ///
    DataBuffer* m_dataActive;

    ///
    static void _cdc_acm_host_dev_callback(const cdc_acm_host_dev_event_data_t* event, void* user_ctx);

    ///
    static bool _cdc_acm_data_callback(const uint8_t* data, size_t data_len, void* arg);

    ///
    void cdc_acm_host_dev_callback(const cdc_acm_host_dev_event_data_t* event);

    ///
    bool cdc_acm_data_callback(const uint8_t* data, size_t data_len);


    ///
    static void _cdc_acm_task(void* arg);

    ///
    void cdc_acm_task();

    ///
    void RemoveQueueData();

    ///
    void Finalize();



public:


    ///
    USBCdcAcm();

    ///
    ~USBCdcAcm();

    ///
    bool Initialize();

    ///
    bool IsConnected();

    ///
    bool Write(uint8_t* buffer, size_t len, uint32_t timeout_ms);
    
    ///
    bool IsAvailable();

    ///
    char Read();
};


