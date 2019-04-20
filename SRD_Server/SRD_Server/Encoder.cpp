#include "Encoder.hpp"

Encoder::Encoder() : errorMessage(nullptr), currentBufferSize(0), buffer(nullptr) { context = ZSTD_createCCtx(); }
Encoder::~Encoder() noexcept { ZSTD_freeCCtx(context); }

void Encoder::reallocateBuffer(const size_t& size) // May throw bad_alloc
{
    if(buffer)
    {
        delete[] buffer;
        buffer = nullptr;
    }
    
    buffer = new uint8_t[size];
}

// Returns buffer that valid only before the next call to this function
std::pair<uint8_t*,size_t> Encoder::encode(uint8_t* source, size_t sourceSize, int compression_level)
{
    size_t recommended = ZSTD_compressBound(sourceSize);
    
    if(currentBufferSize < recommended)
    {
        currentBufferSize = recommended + recommended * 0.1;
        this->reallocateBuffer(currentBufferSize);
    }
    
    size_t compressedRes = ZSTD_compressCCtx(context,
                                             buffer, currentBufferSize,
                                             source, sourceSize,
                                             compression_level);
    
    bool error = ZSTD_isError(compressedRes);
    errorMessage = error ? ZSTD_getErrorName(compressedRes) : "No errors found";
    
    if(error)
    {
        return std::make_pair(nullptr, 0);
    }
    
    return std::make_pair(buffer, compressedRes);
}
