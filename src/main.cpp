#include "Common.h"
#include <M5Unified.h>
#include "PDCamera.h"


///
static PDCamera pdCamera;
static M5GFX gfx;
static USBSerialHost& host = USBSerialHost::GetInstance();



///
void setup()
{
    esp_log_level_set("*", ESP_LOG_NONE);

    auto cfg = M5.config();
    M5.begin(cfg);
    gfx.begin();

    M5.setLogDisplayIndex(0);

    // To supply power to Playdate.
    M5.Power.setUsbOutput(true);

    host.Initialize();
    pdCamera.Initialize();
}


///
void loop()
{
    pdCamera.AppTask();
}



