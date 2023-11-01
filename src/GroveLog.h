#pragma once
#include "common.h"


#ifdef ENABLE_GROVE_LOG
    #define GLInitialize(cfg)       GroveLog::Initialize(cfg)
    #define GLPrintln(msg)          GroveLog::Println(msg)
    #define GLPrintf(format, ...)   GroveLog::Printf(format, __VA_ARGS__)
    #define GLWrite(buffer, size)   GroveLog::Write(buffer, size)
#else
    #define GLInitialize(cfg)       NULL
    #define GLPrintln(msg)          NULL
    #define GLPrintf(format, ...)   NULL
    #define GLWrite(buffer, size)   NULL
#endif



#ifdef ENABLE_GROVE_LOG
///
class GroveLog
{
public:

    enum Config{
        M5StackCoreS3_PortA,
        M5StackCoreS3_PortB,
        M5StackCoreS3_PortC
    };

    ///
    static void Initialize(Config cfg);

    ///
    static void Println(const char* msg);

    ///
    static void Printf(const char* format, ...);

    ///
    static void Write(const uint8_t* buffer, size_t size);
};


#endif // ENABLE_GROVE_LOG





