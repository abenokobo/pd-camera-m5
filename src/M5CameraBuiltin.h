#pragma once
#include "Common.h"
#include "M5CameraIF.h"
#include <esp_camera.h>



///
class M5CameraBuiltin
    : public M5CameraIF
{
private:


    ///
    sensor_t* m_sensor;



public:


    ///
    M5CameraBuiltin();

    ///
    ~M5CameraBuiltin();



    /*
        M5CameraIF
    */


    ///
    virtual bool Initialize();

    ///
    virtual void SetContrast(int contrast);

    ///
    virtual void SetBrightness(int brightness);

    ///
    virtual void SetMirror(int mirror);

    ///
    virtual void Capture(DataBuffer* data);
};


