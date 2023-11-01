#pragma once
#include "M5Unified.h"



///
class M5TouchButton
{
protected:


    ///
    enum EState {
        EPressed,
        EHolding,
        ENone
    } m_state;


    ///
    uint16_t m_left;
    uint16_t m_top;
    uint16_t m_width;
    uint16_t m_height;



public:


    /// 
    M5TouchButton();
    
    /// 
    ~M5TouchButton();

    ///
    void SetRect(uint16_t left, uint16_t top, uint16_t width, uint16_t height);

    ///
    void Update();

    ///
    virtual void OnPressed(uint16_t x, uint16_t y) = 0;

    ///
    virtual void OnHold(uint16_t x, uint16_t y) = 0;

    ///
    virtual void OnDraw(EState state, uint16_t left, uint16_t top, uint16_t width, uint16_t height) = 0;
};


