//
//  JPEG_Encoder.hpp
//  SRD_Server
//
//  Created by Nikita on 4/21/19.
//  Copyright © 2019 Nikita. All rights reserved.
//

#ifndef JPEG_Encoder_hpp
#define JPEG_Encoder_hpp

#include "ImageEncoder.h"
#include <turbojpeg.h>

// JPEG_Encoder IS Encoder
class JPEG_Encoder : public ImageEncoder {

public :
    JPEG_Encoder();
    ~JPEG_Encoder();
    
    void setQuality(const uint8_t& _quality) { quality = _quality; }
    void setPixelFormat(const uint8_t& _pixel_format) { pixel_format = _pixel_format; }
    
    std::pair<u_long, uint8_t*> encode(uint8_t** rawData, const u_long& width, const u_long& height) override;
    
private :
    bool reallocateBuffer(const size_t& size) override;
    
    u_long currentBufferSize;
    uint8_t quality;
    int pixel_format = TJPF_BGRA;
    
    tjhandle encodeHandle;
};

#endif /* JPEG_Encoder_hpp */
