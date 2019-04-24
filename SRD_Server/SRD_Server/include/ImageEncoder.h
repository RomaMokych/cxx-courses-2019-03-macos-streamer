#ifndef ImageEncoder_h
#define ImageEncoder_h

#include "RegularHeaders.h"

class ImageEncoder {
    
public :
    inline ImageEncoder() {};
    virtual ~ImageEncoder() {};
    
    virtual std::pair<u_long, uint8_t*> encode(uint8_t** rawData, const u_long& width, const u_long& height) = 0;
    
    virtual void setQuality(const uint8_t& _quality) = 0;
    virtual void setPixelFormat(const uint8_t& _pixel_format) = 0;
    
    const char* getLastError() const { return errorMessage; }
    
protected :
    const char* errorMessage = "Nothing happened";
    
    size_t currentBufferSize;
    uint8_t* buffer;
    
    virtual bool reallocateBuffer(const size_t& size) = 0;
};

#endif /* ImageEncoder_h */
