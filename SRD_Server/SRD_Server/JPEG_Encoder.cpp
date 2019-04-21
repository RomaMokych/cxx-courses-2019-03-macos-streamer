#include "JPEG_Encoder.hpp"

JPEG_Encoder::JPEG_Encoder()
{
    
}

JPEG_Encoder::~JPEG_Encoder()
{
    
}

// Returns buffer that valid only before the next call to this function
std::pair<u_long, uint8_t*> JPEG_Encoder::encode(uint8_t** rawData, const u_long& width, const u_long& height)
{
    u_long sizeOfCompressed = 0;
    u_long dataSize = width * height * tjPixelSize[pixel_format];
    
    if(currentBufferSize < dataSize)
    {
        currentBufferSize = dataSize + dataSize * 0.1;
        
        if(!this->reallocateBuffer(currentBufferSize))
             return std::make_pair(0, nullptr);
        
    }
 
    encodeHandle = tjInitCompress(); // Should be created each frame
    
    int result = tjCompress2 (encodeHandle,
                              *rawData,
                              width,
                              width * tjPixelSize[pixel_format],
                              height,
                              pixel_format,
                              &buffer,
                              &sizeOfCompressed,
                              TJSAMP_420,
                              quality,
                              NULL);
    
    tjDestroy(encodeHandle);
    
    bool error = result < 0;
    errorMessage = result >= 0 ? "Successfuly converted to JPEG" : "An error occured";
    
    if(error)
    {
        return std::make_pair(0, nullptr);
    }
    
    return std::make_pair(sizeOfCompressed, buffer);
}

bool JPEG_Encoder::reallocateBuffer(const size_t& size)
{
    if(buffer)
    {
        delete[] buffer;
        buffer = nullptr;
    }
    
    try
    {
        buffer = new uint8_t[size];
    }
    catch(std::bad_alloc)
    {
        buffer = nullptr;
        errorMessage = "Cant allocate memory for encoding";
        return false;
    }
    
    return true;
}
