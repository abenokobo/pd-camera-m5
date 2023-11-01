#include "Common.h"
#include "DataBuffer.h"


///
DataBuffer::DataBuffer(size_t len)
: m_buffer(NULL)
, m_nLength(0)
, m_nPos(0)
{
    if (len > 0)
    {
        m_buffer = (uint8_t*)::malloc(len);
        if (m_buffer)
        {
            m_nLength = len;
        }
    }
}


DataBuffer::DataBuffer(void* buffer, size_t len)
: m_buffer(NULL)
, m_nLength(0)
, m_nPos(0)
{
    if (buffer && len > 0)
    {
        m_buffer = (uint8_t*)::malloc(len);
        if (m_buffer)
        {
            ::memcpy(m_buffer, buffer, len);
            m_nLength = len;
        }
    }
}


///
DataBuffer::~DataBuffer()
{
    if (m_buffer)
    {
        ::free(m_buffer);
    }
}


///
bool DataBuffer::IsAvailable()
{
    if (!m_buffer || m_nLength == 0)
    {
        return false;
    }

    if (m_nLength <= m_nPos)
    {
        return false;
    }

    return true;
}


///
bool DataBuffer::Read(char& in)
{
    if (IsAvailable())
    {
        in = m_buffer[m_nPos];
        m_nPos++;
        return true;
    }
    return false;
}



