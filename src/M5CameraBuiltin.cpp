#include <Arduino.h>
#include <M5Unified.h>
#include <esp_camera.h>
#include <vector>
#include "M5CameraBuiltin.h"


///
M5CameraBuiltin::M5CameraBuiltin()
: m_sensor(NULL)
{
}


///
M5CameraBuiltin::~M5CameraBuiltin()
{

}


///
bool M5CameraBuiltin::Initialize()
{
    static camera_config_t camera_config = {
        .pin_pwdn     = -1,
        .pin_reset    = -1,
        .pin_xclk     = 2,
        .pin_sscb_sda = 12,
        .pin_sscb_scl = 11,

        .pin_d7 = 47,
        .pin_d6 = 48,
        .pin_d5 = 16,
        .pin_d4 = 15,
        .pin_d3 = 42,
        .pin_d2 = 41,
        .pin_d1 = 40,
        .pin_d0 = 39,

        .pin_vsync = 46,
        .pin_href  = 38,
        .pin_pclk  = 45,

        .xclk_freq_hz = 20000000,
        .ledc_timer   = LEDC_TIMER_0,
        .ledc_channel = LEDC_CHANNEL_0,

        .pixel_format = PIXFORMAT_GRAYSCALE,
        .frame_size   = FRAMESIZE_QVGA,   // QVGA(320x240)
        .jpeg_quality = 0,
        .fb_count     = 2,
        .fb_location  = CAMERA_FB_IN_PSRAM,
        .grab_mode    = CAMERA_GRAB_WHEN_EMPTY,
    };

    M5.In_I2C.release();

    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK)
    {
        M5.Display.println("Camera Init Failed");
        return false;
    }

    m_sensor = esp_camera_sensor_get();
    if (m_sensor == NULL)
    {
        M5.Display.println("Camera Init Failed");
        return false;
    }

    return true;
}


///
void M5CameraBuiltin::SetContrast(int contrast)
{
    if (m_sensor)
    {
        m_sensor->set_contrast(m_sensor, contrast);
    }
}


///
void M5CameraBuiltin::SetBrightness(int brightness)
{
    if (m_sensor)
    {
        m_sensor->set_brightness(m_sensor, brightness);
    }
}


///
void M5CameraBuiltin::SetMirror(int mirror)
{
    if (m_sensor)
    {
        m_sensor->set_hmirror(m_sensor, mirror);
    }
}


///
void M5CameraBuiltin::Capture(DataBuffer* data)
{
    // acquire a frame
    M5.In_I2C.release();

    assert(data != NULL);

    camera_fb_t* fb = esp_camera_fb_get();
    if (fb)
    {
        if (data->GetSize() < fb->len)
        {
            ::memcpy(data->GetBuffer(), fb->buf, CAMERA_DATA_SIZE_W * CAMERA_DATA_SIZE_H);
        }
        else
        {
            ::memcpy(data->GetBuffer(), fb->buf, fb->len);
        }
        esp_camera_fb_return(fb);
    }
    else
    {
        ::memset(data->GetBuffer(), 0xff,  CAMERA_DATA_SIZE_W * CAMERA_DATA_SIZE_H);
    }
}

