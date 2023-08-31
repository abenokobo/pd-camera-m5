#include <M5Unified.h>
#include "PDCameraTask.h"




///
void PDCameraTask::Capture()
{
    ///
    auto buf = m_bufPool[m_poolIndex++];
    if (m_poolIndex >= CAMERA_QUEUE_POOL_COUNT)
    {
        m_poolIndex = 0;
    }

    m_spCamera->SetBrightness(m_brightness);
    m_spCamera->SetContrast(m_contrast);
    m_spCamera->SetMirror(m_mirror);
    m_spCamera->Capture(buf);

    taskYIELD();

    switch (m_currDitherType)
    {
    case STUCKI:
        m_oDither.StuckiDither(buf->GetBuffer());
        break;

    case ATKINSON:
        m_oDither.AtkinsonDither(buf->GetBuffer());
        break;

    case FS:
        m_oDither.FSDither(buf->GetBuffer());
        break;

    case FAST:
        m_oDither.fastEDDither(buf->GetBuffer());
        break;

    case BAYER:
        m_oDither.patternDither(buf->GetBuffer());
        break;

    case RNDM:
        m_oDither.randomDither(buf->GetBuffer(), false);
        break;

    default:
        m_oDither.thresholding(buf->GetBuffer(), m_thresh);
        break;
    }

    taskYIELD();

    xQueueSendToBack(m_queue, &buf, portMAX_DELAY);
}


///    
#define WAIT_CAPTURE_TIMEOUT 10
void PDCameraTask::CameraCaptureTask()
{
    while (1)
    {
        Capture();
        delay(WAIT_CAPTURE_TIMEOUT);
    }
}


///
void PDCameraTask::_CameraCaptureTask(void* param)
{
    ((PDCameraTask*)param)->CameraCaptureTask();
}


///
PDCameraTask::PDCameraTask()
: m_poolIndex(0)
, m_queue(NULL)
, m_oDither(CAMERA_DATA_SIZE_W, CAMERA_DATA_SIZE_H)
, m_currDitherType(STUCKI)
, m_thresh(128)
, m_contrast(0)
, m_brightness(0)
, m_mirror(0)
{
}


///
PDCameraTask::~PDCameraTask()
{
    if (m_queue)
    {
        vQueueDelete(m_queue);
    }
}


///
void PDCameraTask::BeginTask()
{
#if defined USE_CAMERA_BUILTIN
    m_spCamera = std::make_shared<M5CameraBuiltin>();
#elif defined USE_CAMERA_GROVE
    m_spCamera = std::make_shared<M5CameraGrove>();
#endif

    for (int i = 0; i < CAMERA_QUEUE_POOL_COUNT; i++)
    {
        m_bufPool[i] = new DataBuffer(CAMERA_DATA_SIZE_W * CAMERA_DATA_SIZE_H);
    }
    
    m_queue = xQueueCreate(CAMERA_QUEUE_COUNT, sizeof(void*));

    if (!m_spCamera->Initialize())
    {
        M5.Log.println("Camera return Failed.");
        m_spCamera = NULL;
        return;
    }
    m_oDither.buildBayerPattern();

    xTaskCreatePinnedToCore(&PDCameraTask::_CameraCaptureTask, "cameracapturetask", 4 * 512, this, 1, NULL, PRO_CPU_NUM);

    delay(100);
}


///
void PDCameraTask::SetDitherType(DitherType currDitherType)
{
    m_currDitherType = currDitherType;
}


///
DitherType PDCameraTask::GetDitherType()
{
    return m_currDitherType;
}


///
void PDCameraTask::SetThresh(uint8_t thresh)
{
    m_thresh = thresh;
}


///
void PDCameraTask::SetContrast(int contrast)
{
    m_contrast = contrast;
}


///
void PDCameraTask::SetBrightness(int brightness)
{
    m_brightness = brightness;
}


///
void PDCameraTask::SetMirror(int mirror)
{
    m_mirror = mirror;
}


///
DataBuffer* PDCameraTask::GetCameraData()
{
    void* p = NULL;
    xQueueReceive(m_queue, &p, portMAX_DELAY);
    if (!p)
    {
        return NULL;
    }
    
    taskYIELD();

    return (DataBuffer*)p;
}


