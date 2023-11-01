#pragma once
#include "M5TouchButton.h"


///
class PowerOffButton
: public M5TouchButton
{
public:


    ///
    PowerOffButton();

    ///
    virtual void OnPressed(uint16_t x, uint16_t y);

    ///
    virtual void OnHold(uint16_t x, uint16_t y);

    ///
    virtual void OnDraw(EState state, uint16_t left, uint16_t top, uint16_t width, uint16_t height);
};



