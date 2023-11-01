#include <M5Unified.h>
#include "PDCamera.h"
#include "USBSerialHost.h"


#ifdef ENABLE_PDCAMERA_DETAIL_LOG
    #define INIT_PDCAMERA_LOG()     static int b;
    #define BEGIN_PDCAMERA_LOG()    b = millis();
    #define END_PDCAMERA_LOG(d)     *d = millis() - b;
#else
    #define INIT_PDCAMERA_LOG()
    #define BEGIN_PDCAMERA_LOG()
    #define END_PDCAMERA_LOG(d)
#endif


///
static USBSerialHost& host = USBSerialHost::GetInstance();



///
bool PDCamera::prefix(const char *pre, const char *str)
{
    return strncmp(pre, str, strlen(pre)) == 0;
}


///
const char* PDCamera::dataWithPrefix(const char *prefix, const char *string)
{
    int prefix_length = strlen(prefix);
    int string_length = strlen(string);

    if (prefix_length > string_length)
    {
        return NULL;
    }

    for (int i = 0; i < prefix_length; i++)
    {
        if (string[i] != prefix[i])
        {
            return NULL;
        }
    }

    return string + prefix_length;
}


void PDCamera::sendStatusCode(StatusCode code)
{
    static const int statusCharIndex = 87;
    static std::vector<uint8_t> luaSendStatusCode = {
        27, 76, 117, 97, 84, 0, 25, 147, 13, 10, 26, 10, 4, 4, 4, 120,
        86, 0, 0, 0, 64, 185, 67, 1, 147, 64, 112, 114, 111, 99, 101, 115,
        115, 83, 116, 97, 116, 117, 115, 46, 108, 117, 97, 128, 128, 0, 1, 2,
        133, 79, 0, 0, 0, 9, 0, 0, 0, 131, 128, 0, 0, 66, 0, 2,
        1, 68, 0, 1, 1, 130, 4, 142, 95, 99, 97, 109, 101, 114, 97, 83,
        116, 97, 116, 117, 115, 4, 130, 49, 129, 1, 0, 0, 128, 133, 1, 0,
        0, 0, 0, 128, 128, 129, 133, 95, 69, 78, 86};

    if (m_evalStatusCode == NULL)
    {
        m_evalStatusCode = std::make_shared<PDUSBEval>(luaSendStatusCode.size());
        uint8_t* expBuf = m_evalStatusCode->GetExpressionBuffer();
        ::memcpy(expBuf, luaSendStatusCode.data(), luaSendStatusCode.size());
    }        

    auto expBuf = m_evalStatusCode->GetExpressionBuffer();
    *(expBuf + 87) = code;

    m_evalStatusCode->Eval();
    m_sinceLastMessage = 0;
}


///
void PDCamera::sendImage(DataBuffer* imageBuffer)
{
    static const int headerLength = 86;
    static std::vector<uint8_t> evalPayloadHeader = {
        27, 76, 117, 97, 84, 0, 25, 147, 13, 10, 26, 10, 4, 4, 4, 120,
        86, 0, 0, 0, 64, 185, 67, 1, 146, 64, 112, 114, 111, 99, 101, 115,
        115, 73, 109, 97, 103, 101, 46, 108, 117, 97, 128, 128, 0, 1, 2, 133,
        79, 0, 0, 0, 9, 0, 0, 0, 131, 128, 0, 0, 66, 0, 2, 1,
        68, 0, 1, 1, 130, 4, 141, 95, 99, 97, 109, 101, 114, 97, 73, 109,
        97, 103, 101, 20, 75, 129}; // for 320x240 (QVGA) image
    // 93, 225}; for 400x240 image

    static const int footerLength = 19;
    static std::vector<uint8_t> evalPayloadFooter = {
        129, 1, 0, 0, 128, 133, 1, 0, 0, 0,
        0, 128, 128, 129, 133, 95, 69, 78, 86};

    static const int imageWidth = 320;
    static const int imageHeight = 240;
    static const int packedImageLength = imageWidth * imageHeight / 8;
    static const int payloadLength = headerLength + packedImageLength + footerLength;

    if (m_evalImage == NULL)
    {
        m_evalImage = std::make_shared<PDUSBEval>(payloadLength);
        uint8_t* expBuf = m_evalImage->GetExpressionBuffer();
        ::memcpy(expBuf, evalPayloadHeader.data(), headerLength);
        ::memcpy(expBuf + headerLength + packedImageLength, evalPayloadFooter.data(), footerLength);
    }

    uint8_t* src = imageBuffer->GetBuffer();
    uint8_t* bufImage = m_evalImage->GetExpressionBuffer();
    bufImage += headerLength;

    int byteIndex = 0;
    int pixelIndex = 0;
    int bufferIndex = 0;
    while (byteIndex < packedImageLength)
    {
        byte currentByte = 0;
        for (int n = 7; n >= 0; n--)
        {
            uint8_t val = src[bufferIndex++];
            if (val == 0u)
            {
                // clear bit at byteindex
                currentByte &= ~(1 << n);
            }
            else
            {
                // set bit at byteindex
                currentByte |= (1 << n);
            }
            pixelIndex++;
            pixelIndex = pixelIndex % imageWidth;
        }
        bufImage[byteIndex++] = currentByte;
    }

    if (m_status != SENDING_FRAME)
    {
        return;
    }

    m_evalImage->Eval();
    m_sinceLastMessage = 0;
}


///
bool PDCamera::processDataStatus(const char *data)
{
    // Basic Camera commands.
    if (strcmp(data, "Camera:connect") == 0)
    {
        sendStatusCode(ST_READY);
        m_status = CONNECTED;
        return true;
    }
#ifdef HAS_READ_BUFFER_CORRUPTION_ISSUE
    else if (strstr(data, "NextFrame") != NULL)
#else
    else if (strcmp(data, "Camera:readyForNextFrame") == 0)
#endif
    {
        sendStatusCode(ST_STREAMING);
        m_status = FRAME_REQUESTED;
        //m_oCameraTask.CaptureRequest();
        return true;
    }
    else if (strcmp(data, "Camera:disconnect") == 0)
    {
        m_status = AWAITING_CONNECTION;
        return true;
    }
    return false;
}


///
bool PDCamera::processDataDither(const char* data)
{
    // Dithering mode setting.
    const char *ditherMode = dataWithPrefix("Camera:dither:", data);
    if (ditherMode != NULL)
    {
        if (strcmp(ditherMode, "stucki") == 0)
        {
            m_oCameraTask.SetDitherType(STUCKI);
            // sendStatusCode(ST_OK);
        }
        else if (strcmp(ditherMode, "atkinson") == 0)
        {
            m_oCameraTask.SetDitherType(ATKINSON);
            // sendStatusCode(ST_OK);
        }
        else if (strcmp(ditherMode, "fs") == 0)
        {
            m_oCameraTask.SetDitherType(FS);
            // sendStatusCode(ST_OK);
        }
        else if (strcmp(ditherMode, "fast") == 0)
        {
            m_oCameraTask.SetDitherType(FAST);
            // sendStatusCode(ST_OK);
        }
        else if (strcmp(ditherMode, "random") == 0)
        {
            m_oCameraTask.SetDitherType(RNDM);
            // sendStatusCode(ST_OK);
        }
        else if (strcmp(ditherMode, "bayer") == 0)
        {
            m_oCameraTask.SetDitherType(BAYER);
            // sendStatusCode(ST_OK);
        }
        else if (strcmp(ditherMode, "threshold") == 0)
        {
            m_oCameraTask.SetDitherType(THRESHOLD);
            // sendStatusCode(ST_OK);
        }
        else
        {
            sendStatusCode(ST_ERROR);
        }
        return true;
    }
    return false;
}


///
bool PDCamera::processDataContrast(const char* data)
{
    const char *constrastStr = dataWithPrefix("Camera:contrast:", data);
    if (constrastStr != NULL)
    {
        int contrastValue = atoi(constrastStr);
        if (contrastValue > 0 && contrastValue < 256)
        {
            m_oCameraTask.SetContrast(contrastValue);
            // sendStatusCode(ST_OK);
            return true;
        }
        sendStatusCode(ST_ERROR);
        return true;
    }
    return false;
}


///
bool PDCamera::processDataBrightness(const char* data)
{
    const char *brightnessStr = dataWithPrefix("Camera:brightness:", data);
    if (brightnessStr != NULL)
    {
        int brightnessValue = atoi(brightnessStr);
        if (brightnessValue > 0 && brightnessValue < 256)
        {
            m_oCameraTask.SetBrightness(brightnessValue);
            // sendStatusCode(ST_OK);
            return true;
        }
        sendStatusCode(ST_ERROR);
        return true;
    }
    return false;
}


///
bool PDCamera::processDataThreshold(const char* data)
{
    const char *thresholdStr = dataWithPrefix("Camera:threshold:", data);
    if (thresholdStr != NULL)
        {
        int thresholdValue = atoi(thresholdStr);
        if (thresholdValue > 0 && thresholdValue < 256)
        {
            m_oCameraTask.SetThresh(thresholdValue);
            // sendStatusCode(ST_OK);
            return true;
        }
        sendStatusCode(ST_ERROR);
        return true;
    }
    return false;
}


///
bool PDCamera::processDataMirror(const char* data)
{
    const char *mirrorStr = dataWithPrefix("Camera:mirror:", data);
    if (mirrorStr != NULL)
    {
        int mirrorValue = atoi(mirrorStr);
        if (mirrorValue == 1 || mirrorValue == 0)
        {
            m_oCameraTask.SetMirror(mirrorValue);
            // sendStatusCode(ST_OK);
            return true;
        }
        sendStatusCode(ST_ERROR);
        return true;
    }
    return false;
}


///
void PDCamera::processData(const char *data)
{
    // Early exit if this is not a Camera command message.
    if (!prefix("Camera:", data))
    {
    #ifdef ENABLE_PDCAMERA_DETAIL_LOG
        m_strSkip = data;
    #endif

    #ifndef HAS_READ_BUFFER_CORRUPTION_ISSUE
        return;
    #endif
    }

    m_sinceLastMessage = 0;

    if (processDataStatus(data))
    {
        return;
    }

    if (processDataDither(data))
    {
        return;
    }

    if (processDataContrast(data))
    {
        return;
    }

    if (processDataBrightness(data))
    {
        return;
    }
    
    if (processDataThreshold(data))
    {
        return;
    }

    if (processDataMirror(data))
    {
        return;
    }
}


///
void PDCamera::processIncomingByte(const byte inByte)
{
    static char inputLine[MAX_INPUT];
    static unsigned int inputPos = 0;

    switch (inByte)
    {
    case '\n':                   // end of text
        inputLine[inputPos] = 0; // terminating null byte
        // terminator reached! process inputLine here ...
        processData(inputLine);
        // reset buffer for next time
        inputPos = 0;
        break;
    case '\r': // discard carriage return
        break;
    default:
        // keep adding if not full ... allow for terminating null byte
        if (inputPos < (MAX_INPUT - 1))
            inputLine[inputPos++] = inByte;
        break;
    } // end of switch

} // end of processIncomingByte


///
void PDCamera::processImage()
{
    INIT_PDCAMERA_LOG()

    auto mil = millis();
    m_milProcess = mil - m_milLast;
    m_milLast = mil;

    if (m_status != FRAME_REQUESTED) {
        m_status = FRAME_REQUESTED;
        return;
    }

    m_status = SENDING_FRAME;

    auto data = m_oCameraTask.GetCameraData();
    if (data == NULL)
    {
        M5.Lcd.printf("timeout %d", millis());
        m_status = FRAME_REQUESTED;
        return;
    }

    BEGIN_PDCAMERA_LOG()
    sendImage(data);
    END_PDCAMERA_LOG(&m_milSend)

    m_status = CONNECTED;
}


#ifdef ENABLE_PDCAMERA_DETAIL_LOG
void PDCamera::ShowLog()
{
    static const int INC_Y = 16;
    int y = 0;

    M5.Lcd.setCursor(0, y);
    M5.Lcd.printf("millis %d          \n", millis());
    y += INC_Y;

    M5.Lcd.setCursor(0, y);
    M5.Lcd.printf("Battery %d%%          \n", M5.Power.getBatteryLevel());
    y += INC_Y;

    M5.Lcd.setCursor(0, y);
    M5.Lcd.printf("Process:\t %d ms          \n", m_milProcess);
    y += INC_Y;

    M5.Lcd.printf("FPS:\t %.2f         \n", 1000 / (float)m_milProcess);
    y += INC_Y;

    M5.Lcd.setCursor(0, y);
    M5.Lcd.printf("Send:\t %d ms          \n", m_milSend);
    y += INC_Y;

    M5.Lcd.printf("Skip: %s                                                  \n", m_strSkip.c_str());
    y += INC_Y;

    static const std::string CANERA_STATUS_STRING[] = {
        "INITIALIZING" ,
        "AWAITING_CONNECTION" ,
        "CONNECTED" ,
        "FRAME_REQUESTED" ,
        "SENDING_FRAME",
    };
    M5.Lcd.printf("status:\t %s                              \n", CANERA_STATUS_STRING[m_status].c_str());
    y += INC_Y;
}
#else
void PDCamera::ShowLog()
{
    static const int INC_Y = 20;
    int y = 20;

    // M5.Lcd.setCursor(0, y);
    // M5.Lcd.printf("millis %d          \n", millis());
    // y += INC_Y;

    M5.Lcd.setCursor(0, y);
    M5.Lcd.printf("Battery %d%%          \n", M5.Power.getBatteryLevel());
    y += INC_Y;

    M5.Lcd.setCursor(0, y);
    if (m_milProcess != 0)
    {
        M5.Lcd.printf("FPS:\t %.2f         \n", 1000.f / m_milProcess);
    }
    else
    {
        M5.Lcd.print("FPS:\t                       \n");
    }
}
#endif


/*
    public
*/



///
PDCamera::PDCamera()
: m_status(INITIALIZING)
, m_sinceLastMessage(-1)
, m_milLast(0)
, m_milProcess(0)
#ifdef ENABLE_PDCAMERA_DETAIL_LOG
, m_milSend(0)
#endif
{
}


///
PDCamera::~PDCamera()
{
}


///
void PDCamera::Initialize()
{
    m_oCameraTask.BeginTask();
    m_status = CONNECTED;
}


///
void PDCamera::AppTask()
{
    // Send requested image to the Camera app.
    if (m_status == FRAME_REQUESTED)
    {
        processImage();
    }

    if (m_status == CONNECTED) {
        while (host.IsAvailable())
        {
            processIncomingByte(host.Read());
            taskYIELD();
        }
    }

    ShowLog();
    
    delay(1);
}


