#include "ScreenGrabber.h"
#include "Server.hpp"

using namespace std;

ScreenGrabber::ScreenGrabber(MyClasses::Server& server) : server(server) { }

void ScreenGrabber::StartStream() {
    std::cout << "HELLO WORLD\n";
    /* List displays. */
    CGDirectDisplayID display_ids[5]; /* just a typedef'd uint32_t */
    uint32_t found_displays = 0;
    CGError err = CGGetActiveDisplayList(5, display_ids, &found_displays);
    
    
    
    // SET YOUR SCREEN SIZE HERE. BE CAREFUL! IT MUST BE EQUAL TO THE SIZE GIVEN IN THE CLIENT
    size_t output_width = 1920;
    size_t output_height = 1080;
    // -----------------------------------------------------------------------------------------
    
    
    
    if (kCGErrorSuccess != err) {
        printf("Error: failed to retrieve displays.\n");
        exit(EXIT_FAILURE);
    }
    
    if (0 == found_displays) {
        printf("Error: no active displays found.\n");
        exit(EXIT_FAILURE);
    }
    
    for (uint32_t i = 0; i < found_displays; ++i) {
        printf("Display: #%u, id: %u\n", i, display_ids[i]);
    }
    
    cout << "Width : " << output_width << endl;
    cout << "Height : " << output_height << endl;
    
    //size_t output_width = CGDisplayPixelsWide(display_ids[0]);
    //cout << "W : " << output_width << endl;
    //output_width = 1280;
    
    //size_t output_height = CGDisplayPixelsHigh(display_ids[0]);
    //cout << "H : " << output_height << endl;
    //output_height = 720;
    
    uint32_t pixel_format = 'BGRA';
    
    dispatch_queue_t dq = dispatch_queue_create("com.domain.screenstreamermy", DISPATCH_QUEUE_SERIAL);
    
  
     /* because this variable is used in a dispatch_queue we need to use __block because we change it's value inside the block. */
    
    sref = CGDisplayStreamCreateWithDispatchQueue(display_ids[0],
                                                  output_width,
                                                  output_height,
                                                  pixel_format,
                                                  NULL,
                                                  dq,
                                                  ^(CGDisplayStreamFrameStatus status,    /* kCGDisplayStreamFrameComplete, *FrameIdle, *FrameBlank, *Stopped */
                                                    uint64_t time,                        /* Mach absolute time when the event occurred. */
                                                    IOSurfaceRef frame,                   /* opaque pixel buffer, can be backed by GL, CL, etc.. This may be NULL in some cases. See the docs if you want to keep access to this. */
                                                    CGDisplayStreamUpdateRef ref)
                                                  {
                                                      
                                                      uint64_t d = 0;
                                                      
                                                      if (0 != prev_time) {
                                                          d = time - prev_time;
                                                          printf("[Grabber]Time to capture the frame : %llu ms.\n", (d / 1'000'000ull));
                                                      }
                                                      
                                            
                                                      if (kCGDisplayStreamFrameStatusFrameComplete == status
                                                          && NULL != frame)
                                                      {
                                                        
                                                          IOSurfaceLock(frame, kIOSurfaceLockReadOnly, NULL);
                                                          
                                                          uint8_t* pix = (uint8_t*)IOSurfaceGetBaseAddress(frame);
                                                          
                                                          if (NULL != pix) {
                                                              
                                                              server.SendAll(pix, output_width*output_height*4);
                                                              
                                                          }else{
                                                              cout << "Got NULL!\n";
                                                          }
                                                          
                                                          IOSurfaceUnlock(frame, kIOSurfaceLockReadOnly, NULL);
                                                      }
                                                      prev_time = time;
                                                
                                                  }
                                                  );
    
    err = CGDisplayStreamStart(sref);
    
    if (kCGErrorSuccess != err) {
        printf("Error: failed to start streaming the display.\n");
        exit(EXIT_FAILURE);
    }
    printf("\n\n");
}

    void ScreenGrabber::EndStream(){
        CGError err = CGDisplayStreamStop(sref);
        if (kCGErrorSuccess != err) {
            printf("Error: failed to stop streaming the display.\n");
            exit(EXIT_FAILURE);
        }
    }
    
