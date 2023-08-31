#include "PDUSBEval.h"
#include "M5Unified.h"
#include <string.h>


///
static USBSerialHost& host = USBSerialHost::GetInstance();


///
PDUSBEval::PDUSBEval(size_t sizeExpression)
{
    std::string strCommand = "eval ";
    strCommand += std::to_string(sizeExpression);
    strCommand += "\n";
    m_sizeCommand = strCommand.length();

    m_buffer.resize(m_sizeCommand + sizeExpression);
    ::memcpy(m_buffer.data(), strCommand.c_str(), m_sizeCommand);
}


///
uint8_t* PDUSBEval::GetExpressionBuffer()
{
    if (m_buffer.empty())
    {
        return NULL;
    }

    return m_buffer.data() + m_sizeCommand;
}


///
#define EVAL_RETRY_CNT 100
#define EVAL_WAIT_SEC 10
bool PDUSBEval::Eval()
{
    if (m_buffer.empty())
    {
        return false;
    }

    auto semaphore = host.Write(m_buffer.data(), m_buffer.size());
    if (semaphore == NULL)
    {
        return false;
    }

    xSemaphoreTake(semaphore, portMAX_DELAY);
    vSemaphoreDelete(semaphore);

    taskYIELD();

    return true;
}



