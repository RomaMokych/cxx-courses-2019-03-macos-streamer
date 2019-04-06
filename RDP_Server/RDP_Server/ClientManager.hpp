//
//  ClientManager.hpp
//  RDP_Server
//
//  Created by Nikita on 4/7/19.
//  Copyright © 2019 Nikita_Dik. All rights reserved.
//

#ifndef ClientManager_hpp
#define ClientManager_hpp

#include "RegularHeaders.h"
#include "NetworkHeaders.h"

#include "ClientHandler.hpp"

class ClientManager
{
public:
    
    ClientManager(const SocketAddress& serverAddress);
    ~ClientManager();
    
private :
    ServerSocket server;
    SocketReactor reactor;
    SocketAcceptor<ClientHandler>* acceptor;
    
    Thread serverThread;
};
#endif /* ClientManager_hpp */