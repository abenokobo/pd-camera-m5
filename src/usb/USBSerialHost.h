#pragma once
#include "common.h"
#include "USBCdcAcm.h"



class USBSerialHost {
protected:


    ///
    USBCdcAcm m_acm;

    ///
    USBSerialHost();

    ///
    static void _usb_lib_task(void* arg);

    ///
    void usb_lib_task();



public:


    ///
    ~USBSerialHost();

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

    ///
    static USBSerialHost& GetInstance();
};




