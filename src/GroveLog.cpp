#include "GroveLog.h"
#ifdef ENABLE_GROVE_LOG
#include <M5Unified.h>


///
void GroveLog::Initialize(Config cfg)
{
    switch (cfg)
    {
    case M5StackCoreS3_PortA:
        Serial1.begin(115200, SERIAL_8N1, G1, G2);
        break;
    case M5StackCoreS3_PortB:
        Serial1.begin(115200, SERIAL_8N1, G8, G9);
        break;
    case M5StackCoreS3_PortC:
        Serial1.begin(115200, SERIAL_8N1, G18, G17);
        break;
    default:
        assert(false);
        break;
    }
}


///
void GroveLog::Println(const char* msg)
{
    Serial1.write(msg);
    Serial1.write("\n");
}


///
#define _BUFFER_SIZE 1024
static char _buffer[_BUFFER_SIZE];
void GroveLog::Printf(const char* format, ...)
{
    va_list list;
    va_start(list, format);
    vsnprintf(_buffer, _BUFFER_SIZE, format, list);
    Serial1.write(_buffer);
    va_end(list);
}


///
void GroveLog::Write(const uint8_t* buffer, size_t size)
{
    Serial1.write(buffer, size);
}


#endif // ENABLE_GROVE_LOG
