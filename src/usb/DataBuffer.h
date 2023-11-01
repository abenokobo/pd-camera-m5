#pragma once



class DataBuffer {
protected:


    ///
    uint8_t* m_buffer;

    ///
    size_t m_nLength;

    ///
    int m_nPos;



public:


    ///
    DataBuffer(size_t len);

    ///
    DataBuffer(void* buffer, size_t len);

    ///
    ~DataBuffer();

    ///
    bool IsAvailable();

    ///
    bool Read(char& in);

    ///
    size_t GetSize() { return m_nLength; }

    ///
    uint8_t* GetBuffer() { return m_buffer; }
};



