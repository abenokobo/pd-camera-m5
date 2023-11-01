#pragma once
#include "Common.h"
#include "DataBuffer.h"



///
class M5CameraIF
{
public:


    ///
    virtual bool Initialize() = 0;

    ///
    virtual void SetContrast(int contrast) = 0;

    ///
    virtual void SetBrightness(int brightness) = 0;

    ///
    virtual void SetMirror(int mirror) = 0;

    ///
    virtual void Capture(DataBuffer* data) = 0;
};


