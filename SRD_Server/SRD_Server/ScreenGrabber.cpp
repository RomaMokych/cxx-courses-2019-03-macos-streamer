#include "ScreenGrabber.hpp"
#include "SRD_Server.hpp"

ScreenGrabber::ScreenGrabber(SRD_Server& Server) : server(Server), working(false)
{
}

void ScreenGrabber::beginScreenCapturing()
{
    streamScreen = CGDisplayStreamCreateWithDispatchQueue(display,
                                                          output_width,
                                                          output_height,
                                                          pixel_format,
                                                          NULL,         // Additional params
                                                          dispatchQueue,
                                                          ^(CGDisplayStreamFrameStatus status,    /* kCGDisplayStreamFrameComplete, *FrameIdle, *FrameBlank, *Stopped */
                                                            uint64_t time,                        /* Mach absolute time when the event occurred. */
                                                            IOSurfaceRef frame,                   /* opaque pixel buffer, can be backed by GL, CL, etc.. This may be NULL in some cases. See the docs if you want to keep access to this. */
                                                            CGDisplayStreamUpdateRef ref)
                                                          {

                                                              uint64_t tid;
                                                              pthread_threadid_np(NULL, &tid);
                                                             // printf("HELLO from thread GRABBER with id : %ld\n", tid);
                                                              
                                                              uint64_t d = 0;

                                                              if (0 != prev_time) {
                                                                  d = time - prev_time;
                                                              //    printf("[Grabber]Time to capture the frame : %llu ms.\n", (d / 1'000'000ull));
                                                              }


                                                              if (kCGDisplayStreamFrameStatusFrameComplete == status
                                                                  && NULL != frame)
                                                              {

                                                                  IOSurfaceLock(frame, kIOSurfaceLockReadOnly, NULL);

                                                                  uint8_t* pix = (uint8_t*)IOSurfaceGetBaseAddress(frame);

                                                                  if (NULL != pix) {

                                                                      server.sendFrame(pix, output_width, output_height);

                                                                  }else{
                                                                      cout << "Pixel data is null!\n";
                                                                  }

                                                                  IOSurfaceUnlock(frame, kIOSurfaceLockReadOnly, NULL);
                                                              }
                                                              prev_time = time;

                                                          }
                                                          );
}

bool ScreenGrabber::start()
{
    if(working)
        return false;
    
    cout << "Starting ScreenGrabber..\n";
    
    display = CGMainDisplayID();

    cout << "Width : " << output_width << endl;
    cout << "Height : " << output_height << endl;

    dispatchQueue = dispatch_queue_create("com.domain.screenstreamermy", DISPATCH_QUEUE_SERIAL);

    beginScreenCapturing();
    auto error = CGDisplayStreamStart(streamScreen);

    if (kCGErrorSuccess != error) {
        printf("Error: failed to start streaming the display.\n");
        exit(EXIT_FAILURE);
    }
    cout << "Successfuly started!\n";
    working = true;
    return true;
}

bool ScreenGrabber::stop()
{
    if(!working)
        return false;
    
    CGError error = CGDisplayStreamStop(streamScreen);
    if (kCGErrorSuccess != error) {
        printf("Error: failed to stop streaming the display.\n");
        exit(EXIT_FAILURE);
    }
    
    working = false;
    return true;
}
