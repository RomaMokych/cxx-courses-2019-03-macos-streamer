#ifndef ScreenGrabber_h
#define ScreenGrabber_h

#include <stdlib.h>
#include <unistd.h>
#include <CoreGraphics/CGDisplayStream.h>
#include <stdio.h>
#include <IOSurface/IOSurface.h>
#include <iostream>

namespace MyClasses{
    class Server;
}

class ScreenGrabber{
    
    private :
    __block uint64_t prev_time = 0;
    
    CGDisplayStreamRef sref;
    MyClasses::Server& server;
    
    public :
    ScreenGrabber(MyClasses::Server& server);
    
    void StartStream();
    void EndStream();
    
};

#endif /* ScreenGrabber_h */
