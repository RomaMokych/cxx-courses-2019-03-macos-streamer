#ifndef SRD_Server_hpp
#define SRD_Server_hpp
#include <memory>

#include "RegularHeaders.h"
#include "NetworkHeaders.h"

#include "ServerWorker.hpp"
#include "InputManager.hpp"

class ServerWorker;

class SRD_Server
{
public :
    SRD_Server(shared_ptr<ServerWorker>);
    
    bool start();
    bool stop();
    
    void sendFrame(UInt8* frame, int width, int height);
    
    ~SRD_Server();
    
private :
    SocketAddress address;
    shared_ptr<InputManager> inManager;
    shared_ptr<ServerWorker> serverWorker;
    
    Thread* workerThread;
    
    bool working;
    
};

#endif /* SRD_Server_hpp */

