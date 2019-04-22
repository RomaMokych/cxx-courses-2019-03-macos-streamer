#ifndef ScreenGrabber_hpp
#define ScreenGrabber_hpp

#include <IOSurface/IOSurfaceRef.h>
#include <CoreGraphics/CGDisplayStream.h>

#include "RegularHeaders.h"
#include "ServerWorker.hpp"
#include "JPEG_Encoder.hpp"

class ServerWorker;

class ScreenGrabber
{
public :
    ScreenGrabber(shared_ptr<ServerWorker>);
    ~ScreenGrabber();
    
    bool start();
    bool stop();
    
    void beginScreenCapturing();
    
private :
    
     bool working, handlerFinished;
    __block uint64_t prev_time = 0;
    
    // Temporary only ServerWorker can be the output class
    shared_ptr<ServerWorker> output;
    
    CGDisplayStreamRef streamScreen;
    CGDirectDisplayID display;
    
    // Should be set via settings
    u_long output_width = 1920;
    u_long output_height = 1080;
    
    uint32_t pixel_format = 'BGRA';
    
    dispatch_queue_t dispatchQueue;
    
    ImageEncoder* encoder;
};


#endif /* ScreenGrabber_hpp */
