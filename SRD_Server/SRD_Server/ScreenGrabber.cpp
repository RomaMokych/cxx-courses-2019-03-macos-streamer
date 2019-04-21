#include "ScreenGrabber.hpp"
#include "SRD_Server.hpp"


ScreenGrabber:: ScreenGrabber(shared_ptr<ServerWorker> serverWorker) :
                                                output(serverWorker),
                                                working(false),
                                                handlerFinished(true)
{
    jpegBuffer = new uint8_t[output_height*output_width*4];
}

ScreenGrabber::~ScreenGrabber()
{
    delete[] jpegBuffer;
    cout << "ScreenGrabber has been destroyed!\n";
}

void ScreenGrabber::beginScreenCapturing()
{
    
    void* keys[2];
    void* values[2];
    
    keys[0] = (void *) kCGDisplayStreamShowCursor;
    values[0] = (void *) kCFBooleanTrue;
    
    /* --------------------------------------------------------------------------------------------------------------------------- */
    /* Doesen't work -> can't apply YCbCr 4:2:0 (which produces only 1/4 or original image, but keeps it unchanged for human eye!) */
    /* --------------------------------------------------------------------------------------------------------------------------- */
    keys[1] = (void *) kCGDisplayStreamYCbCrMatrix;
    values[1] = (void *) kCGDisplayStreamYCbCrMatrix_ITU_R_601_4;
    /* --------------------------------------------------------------------------------------------------------------------------- */
    
    CFDictionaryRef opts = CFDictionaryCreate(kCFAllocatorDefault, (const void **) keys, (const void **) values, 2, NULL, NULL);
    
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
      
      if(kCGDisplayStreamFrameStatusStopped == status) // Doesn't help in case of force quit
      {
          handlerFinished = true;
          return;
      }
      
      uint64_t difference = 0;

      if (0 != prev_time) {
          difference = time - prev_time;
          
          printf("[Grabber]Time to capture the frame : %llu ms.\n", (difference / 1'000'000ull));
      }
      
      if (kCGDisplayStreamFrameStatusFrameComplete == status && NULL != frame)
      {
          IOSurfaceLock(frame, kIOSurfaceLockReadOnly, NULL);

          uint8_t* rawFrameBuffer = (uint8_t*)IOSurfaceGetBaseAddress(frame);
          u_long sizeOfCompressed = 0, quality = 90;
          
          if (NULL != rawFrameBuffer) {

              int pixel_format = TJPF_BGRA;
              auto startOfCompression = chrono::high_resolution_clock::now();
              
              compressHandle = tjInitCompress(); // Should be created each frame
              
              /* In my virtualBox, CGDisplayStream can't get the image with resolution other than native (1920x1080),
                 so I testing 640x360 compression only on a part of image */
              
              
              /* Compresses whole BGRA image to JPEG format using convertion to YCbCr 4:2:0.
                 Usually all process takes 2-6 ms , but can fluctuate from 6 ms to 30 ms sometimes.
               Size of compressed image with this settings is about 70-80 Kbytes */
              int result = tjCompress2 (compressHandle,
                                        rawFrameBuffer,
                                        640,
                                        640 * tjPixelSize[pixel_format],
                                        360,
                                        pixel_format,
                                        &jpegBuffer,
                                        &sizeOfCompressed,
                                        TJSAMP_420,
                                        quality,
                                        NULL);
             
              tjDestroy(compressHandle);
              
              auto endOfCompression = chrono::high_resolution_clock::now();
              double timeToCompress = chrono::duration_cast<chrono::milliseconds>(endOfCompression - startOfCompression).count();
              
              cout << (result >= 0 ? "Successfuly converted to JPEG" : "An error occured") <<"\n";
              
              cout << "Time took to compress from " << (output_width * tjPixelSize[pixel_format] * output_height)/1024 << " KBytes"
              << " to " << sizeOfCompressed/1024 <<  " Kbytes :" << timeToCompress << " ms\n";
              
              // output->sendFrame(rawFrameBuffer, output_width, output_height);

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
    
    handlerFinished = false;
    
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
    
   // uint64_t wait_total = 0, max_wait = 1'000*2; // Maximumu wait - 2 seconds
    
    // Wait for handler to be done
    while(!handlerFinished) //|| wait_total < max_wait)
    {
       // usleep(10'000);
       // wait_total += 10'000;
        //       cout << "WAIT\n";
    }
    handlerFinished = false;
    
    return true;
}
