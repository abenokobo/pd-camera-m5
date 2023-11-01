#include "PowerOffButton.h"


#define LCD_WIDTH 320
#define LCD_HEIGHT 240

#define BUTTON_WIDTH 128
#define BUTTON_HEIGHT 48
#define BUTTON_MARGIN 10


//#define ENABLE_DOWNLOAD_BOOT
#ifdef ENABLE_DOWNLOAD_BOOT
extern "C" void usb_dc_check_poll_for_interrupts();
extern "C" int usb_dc_prepare_persist(void);
extern "C" void chip_usb_set_persist_flags( uint32_t x );
#define USBDC_PERSIST_ENA (1<<31)
#endif


///
PowerOffButton::PowerOffButton()
{
    SetRect(
        LCD_WIDTH - BUTTON_MARGIN - BUTTON_WIDTH,
        LCD_HEIGHT - BUTTON_MARGIN - BUTTON_HEIGHT,
        BUTTON_WIDTH,
        BUTTON_HEIGHT);
}


///
void PowerOffButton::OnPressed(uint16_t x, uint16_t y)
{
}


///
void PowerOffButton::OnHold(uint16_t x, uint16_t y)
{
#ifdef ENABLE_DOWNLOAD_BOOT
    // Do not work ...
    
    // usb_dc_check_poll_for_interrupts();
    // usb_dc_prepare_persist();
    // chip_usb_set_persist_flags(USBDC_PERSIST_ENA);
    // REG_WRITE(RTC_CNTL_OPTION1_REG, RTC_CNTL_FORCE_DOWNLOAD_BOOT);
    // esp_restart();
#else
    M5.Power.powerOff();
#endif
}


///
void PowerOffButton::OnDraw(EState state, uint16_t left, uint16_t top, uint16_t width, uint16_t height)
{
    int color = TFT_WHITE;
    if (state == EPressed)
    {
        color = TFT_RED;
    }

    M5.Lcd.drawString(
        "Hold to turn off",
        m_left + 15,
        m_top + 21);
    M5.Lcd.drawRoundRect(m_left, m_top, m_width, m_height, 5, color);
}


