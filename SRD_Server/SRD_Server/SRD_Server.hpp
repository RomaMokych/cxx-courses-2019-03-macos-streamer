//
//  SRD_Server.hpp
//  SRD_Server
//
//  Created by Nikita on 4/8/19.
//  Copyright © 2019 Nikita. All rights reserved.
//

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
    
    void sendFrame();
    
    ~SRD_Server();
    
private :
    SocketAddress address;
    ServerSocket* serverSocket;
    
    ScreenGrabber* screenGrabber;
    ServerWorker* serverWorker;
    
    Thread* workerThread;
    
    bool working;
    
};

#endif /* SRD_Server_hpp */

