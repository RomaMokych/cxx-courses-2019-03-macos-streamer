#ifndef ScreenGrabber_hpp
#define ScreenGrabber_hpp

#include <IOSurface/IOSurfaceRef.h>
#include <CoreGraphics/CGDisplayStream.h>

#include "RegularHeaders.h"

class SRD_Server;

class ScreenGrabber
{
public :
    ScreenGrabber(SRD_Server& server);
    bool start();
    bool stop();
    
    void beginScreenCapturing();
    
private :
    SRD_Server& server;
    
    bool working;
    
    __block int prev_time = 0;
    
    CGDisplayStreamRef streamScreen;
    CGDirectDisplayID display;
    
    // Should be set via settings
    size_t output_width = 1920;
    size_t output_height = 1080;
    
    uint32_t pixel_format = 'BGRA';
    
    dispatch_queue_t dispatchQueue;
};


#endif /* ScreenGrabber_hpp */
