#pragma once
#include "Common.h"


#define USE_CAMERA_BUILTIN
//#define USE_CAMERA_GROVE


#if defined USE_CAMERA_BUILTIN
    #include "M5CameraBuiltin.h"
#elif defined USE_CAMERA_GROVE
    #include "M5CameraGrove.h"
#endif

#include <Dither.h>


///
enum DitherType
{
    STUCKI,
    ATKINSON,
    FS,
    FAST,
    BAYER,
    RNDM,
    THRESHOLD
};


#define CAMERA_QUEUE_COUNT 1
#define CAMERA_QUEUE_POOL_COUNT (CAMERA_QUEUE_COUNT + 2)


///
class PDCameraTask
{
protected:


    ///
    DataBuffer* m_bufPool[CAMERA_QUEUE_POOL_COUNT];

    ///
    int m_poolIndex;

    ///
    QueueHandle_t m_queue;

    ///
    Dither m_oDither;

    ///
    DitherType m_currDitherType;

    ///
    uint8_t m_thresh;

    ///
    int m_contrast;

    ///
    int m_brightness;

    ///
    int m_mirror;


    ///
    std::shared_ptr<M5CameraIF> m_spCamera;

    ///
    void Capture();

    ///
    void CameraCaptureTask();

    ///
    static void _CameraCaptureTask(void* param);



public:


    ///
    PDCameraTask();

    ///
    ~PDCameraTask();

    ///
    void BeginTask();

    ///
    void SetDitherType(DitherType currDitherType);

    ///
    DitherType GetDitherType();

    ///
    void SetThresh(uint8_t thresh);    

    ///
    void SetContrast(int contrast);

    ///
    void SetBrightness(int brightness);

    ///
    void SetMirror(int mirror);

    ///
    DataBuffer* GetCameraData();
};


