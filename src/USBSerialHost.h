#pragma once
#include "Common.h"
#include "ModUSBHost.h"
#include "ModUSBacmDevice.h"
#include <memory.h>
#include "M5Unified.h"



class USBSerialHost {
protected:


    ///
    QueueHandle_t m_queue;

    ///
    DataBuffer* m_dataActive;

    ///
    ModUSBhost m_oHost;

    ///
    ModUSBacmDevice* m_opAcmDevice;


    ///
    void acm_events(int event, void* data, size_t len);

    ///
    static void _acm_events(int event, void* data, size_t len);

    ///
    void client_event_callback(const usb_host_client_event_msg_t* event_msg, void *arg);
    
    ///
    static void _client_event_callback(const usb_host_client_event_msg_t* event_msg, void* arg);

    ///
    USBSerialHost();



public:


    
    ///
    ~USBSerialHost();

    ///
    bool Initialize();

    ///
    bool IsConnected();

    ///
    SemaphoreHandle_t Write(uint8_t* buffer, size_t len);
    
    ///
    bool IsAvailable();

    ///
    char Read();

    ///
    void USBHostTask();

    ///
    static void _USBHostTask(void* param);

    ///
    static USBSerialHost& GetInstance();
};


