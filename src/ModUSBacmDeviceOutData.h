#pragma once
#include "usb_device.hpp"
#include "usb_requests.hpp"
#include <semphr.h>


#define TRANSFER_ALLOC_SIZE_READ  (512)
#define TRANSFER_ALLOC_SIZE_WRITE  (4096)



class ModUSBacmDeviceOutData {
public:


    ///
    usb_transfer_t* m_xfer_write;

    ///
    uint8_t* m_OutData;

    ///
    size_t m_OutRemain;

    ///
    bool m_bCom;

    ///
    SemaphoreHandle_t m_semaphore;



 public:


    ///
    ModUSBacmDeviceOutData();

    ///
    ~ModUSBacmDeviceOutData();

    ///
    SemaphoreHandle_t Initialize(usb_transfer_t* write, uint8_t* data, size_t len);

    ///
    void OnOutDataCB();

    ///
    bool HasRemainDataAsync();

    ///
    bool DoOutData();
};



