#pragma once
#include "common.h"
#include "PDCameraTask.h"


#include "PDUSBEval.h"


//#define ENABLE_PDCAMERA_DETAIL_LOG



/*
    [Memo]
    Fix to the problem that the read buffer can be corrupted when receiving the status to send the next camera image after the camera image has been sent.
    Delete this definition once the buffer corruption problem is resolved.

    カメラ画像送信後、次のカメラ画像を送信するためのステータスを受け取るときに、読み込みバッファが破損することがある問題への対策。
    バッファが破損の問題が解決したらこの定義を削除する。
*/
//#define HAS_READ_BUFFER_CORRUPTION_ISSUE


///
class PDCamera
{
private:


    ///
    enum StatusCode
    {
        ST_INIT = 48,         // 0 in ASCII
        ST_READY = 49,        // 1
        ST_STREAMING = 50,    // 2
        ST_OK = 51,           // 3
        ST_ERROR = 52         // 4
    };

    ///
    PDCameraTask m_oCameraTask;

    ///
    enum CameraStatus {
        INITIALIZING,
        AWAITING_CONNECTION,
        CONNECTED,
        FRAME_REQUESTED,
        SENDING_FRAME,
    };
    CameraStatus m_status;

    ///
    int m_sinceLastMessage;


    unsigned long m_milLast;

    unsigned long m_milProcess;

    unsigned long m_milSend;

#ifdef ENABLE_PDCAMERA_DETAIL_LOG
    std::string m_strSkip;
#endif


    ///
    static bool prefix(const char *pre, const char *str);

    ///
    static const char* dataWithPrefix(const char *prefix, const char *string);

    ///
    std::shared_ptr<PDUSBEval> m_evalStatusCode;
    void sendStatusCode(StatusCode code);

    ///
    std::shared_ptr<PDUSBEval> m_evalImage;
    void sendImage(DataBuffer* imageByte);

    ///
    bool processDataStatus(const char *data);

    ///
    bool processDataDither(const char* data);

    ///
    bool processDataContrast(const char* data);

    ///
    bool processDataBrightness(const char* data);
    
    ///
    bool processDataThreshold(const char* data);

    ///
    bool processDataMirror(const char* data);

    ///
    void processData(const char *data);

    ///
    void processIncomingByte(const byte inByte);

    ///
    void processImage();

    ///
    void ShowLog();



public:


    ///
    PDCamera();

    ///
    ~PDCamera();

    ///
    void Initialize();

    ///
    void AppTask();
};


