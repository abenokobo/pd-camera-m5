#include "M5TouchButton.h"


/// 
M5TouchButton::M5TouchButton()
: m_left(0)
, m_top(0)
, m_width(0)
, m_height(0)
, m_state(ENone)
{
}


/// 
M5TouchButton::~M5TouchButton()
{

}


///
void M5TouchButton::SetRect(uint16_t left, uint16_t top, uint16_t width, uint16_t height)
{
    m_left = left;
    m_top = top;
    m_width = width;
    m_height = height;
}


///
void M5TouchButton::Update()
{
    m_state = ENone;
    if (M5.Touch.isEnabled())
    {
        auto t = M5.Touch.getDetail();
        if ((m_left <= t.x) && (m_left + m_width >= t.y) &&
            (m_top <= t.y) && (m_top + m_height >= t.y))
        {
            if (t.isHolding())
            {
                OnHold(t.x, t.y);
                m_state = EHolding;
            }
            else
            if (t.isPressed())
            {
                OnPressed(t.x, t.y);
                m_state = EPressed;
            }
        }
    }

    OnDraw(m_state, m_left, m_top, m_width, m_height);
}


