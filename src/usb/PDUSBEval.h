#pragma once
#include "Common.h"
#include "USBSerialHost.h"



///
class PDUSBEval {
private:


    ///
    size_t m_sizeCommand;

    ///
    std::vector<uint8_t> m_buffer;



public:


    ///
    PDUSBEval(size_t sizeExpression);

    ///
    uint8_t* GetExpressionBuffer();

    ///
    bool Eval();

    ///
    static void MakeLuaStringInstruction(size_t length, std::vector<uint8_t>& instruction);
};


