#pragma once
#include "ModUSBacmDeviceOutData.h"
#include "usb_device.hpp"
#include "usb_requests.hpp"
#include <memory>



///
#define CDC_DATA_IN                             1
#define CDC_DATA_OUT                            2
#define CDC_CTRL_SET_CONTROL_LINE_STATE         3
#define CDC_CTRL_SET_LINE_CODING                4
#define CDC_CTRL_GET_LINE_CODING                5




///
typedef void (*cdc_event_cb_t)(int, void* data, size_t len);
class ModUSBacmDevice : public USBhostDevice
{
public:


    ///
    const usb_ep_desc_t * ep_int;

    ///
    const usb_ep_desc_t * ep_in;

    ///
    const usb_ep_desc_t * ep_out;

    ///
    cdc_event_cb_t event_cb = nullptr;

    ///
    bool connected;

    ///
    std::shared_ptr<ModUSBacmDeviceOutData> m_spOutData;

    ///
    ModUSBacmDevice();

    ///
    void OutDataAsyncImpl();



public:

    ///
    ModUSBacmDevice(const usb_config_desc_t* config_desc, const usb_intf_desc_t *intf, size_t n, USBhost* host);

    ///
    ~ModUSBacmDevice();

    ///
    bool init();

    ///
    void setControlLine(bool dtr, bool rts);

    ///
    void setLineCoding(uint32_t bitrate, uint8_t cf, uint8_t parity, uint8_t bits);

    ///
    void getLineCoding();

    ///
    void INDATA();

    ///
    SemaphoreHandle_t OutDataAsync(uint8_t*, size_t);

    ///
    void OnOutDataCB();

    ///
    void DoOutData();

    ///
    void onEvent(cdc_event_cb_t _cb);

    ///
    bool isConnected();

    ///
    void _callback(int event, usb_transfer_t* data);
};

