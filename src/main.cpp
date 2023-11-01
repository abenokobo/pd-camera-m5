#include "Common.h"
#include <M5Unified.h>
#include "PDCamera.h"
#include "PowerOffButton.h"


///
static PDCamera pdCamera;
static USBSerialHost& host = USBSerialHost::GetInstance();
static PowerOffButton offBtn;


///
void setup()
{
    esp_log_level_set("*", ESP_LOG_NONE);

    auto cfg = M5.config();
    M5.begin(cfg);

    GLInitialize(GroveLog::M5StackCoreS3_PortB);
    GLPrintln("== Begin pd-camera-m5 ==");

    // To supply power to Playdate.
    M5.Power.setUsbOutput(true);

    host.Initialize();
    pdCamera.Initialize();
}


///
void loop()
{
    M5.update();
    offBtn.Update();
    
    pdCamera.AppTask();
}


