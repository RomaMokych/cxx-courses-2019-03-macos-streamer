#ifndef Encoder_hpp
#define Encoder_hpp

#include <iostream>
#include <zstd.h>

using namespace std;

class Encoder
{
    public :
    Encoder();
    ~Encoder();
    
    std::pair<uint8_t*,size_t> encode(uint8_t* source, size_t sourceSize, int compression_level);
    const char* getLastError() const { return errorMessage; }
    
    private :
    ZSTD_CCtx* context;
    const char* errorMessage = "Nothing happened";
    
    size_t currentBufferSize;
    uint8_t* buffer;
    
    void reallocateBuffer(const size_t& size);
};

#endif /* Encoder_hpp */
