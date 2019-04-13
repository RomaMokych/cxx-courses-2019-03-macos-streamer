#ifndef SRD_Server_hpp
#define SRD_Server_hpp

#include "RegularHeaders.h"
#include "NetworkHeaders.h"
#include "ServerWorker.hpp"

class ScreenGrabber;
class ServerWorker;

class SRD_Server
{
public :
    SRD_Server(const SocketAddress&);
    
    bool start();
    bool stop();
    
    void sendFrame(UInt8* frame, int width, int height);
    
    ~SRD_Server();
    
private :
    SocketAddress address;
    ServerSocket* serverSocket;
    
    ScreenGrabber* screenGrabber;
    ServerWorker* serverWorker;
    
    Thread* workerThread;
    
    bool working;
    
    const unsigned int max_screenFrameMessage_size, header_size;
    UInt8* screenFrameMessage;
    
};

#endif /* SRD_Server_hpp */

