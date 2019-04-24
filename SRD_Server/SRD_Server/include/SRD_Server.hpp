#ifndef SRD_Server_hpp
#define SRD_Server_hpp
#include <memory>

#include "InputManager.hpp"

#include "RegularHeaders.h"
#include "NetworkHeaders.h"

#include "ServerWorker.hpp"

class ServerWorker;

class SRD_Server
{
public :
    SRD_Server(shared_ptr<ServerWorker>);
    
    bool start();
    bool stop();

    ~SRD_Server();
    
private :
    SocketAddress address;
    shared_ptr<InputManager> inputManager;
    shared_ptr<ServerWorker> serverWorker;
    
    Thread* workerThread;
    
    bool working;
    
};

#endif /* SRD_Server_hpp */

