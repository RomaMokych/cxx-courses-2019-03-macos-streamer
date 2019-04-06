//
//  ClientManager.cpp
//  RDP_Server
//
//  Created by Nikita on 4/7/19.
//  Copyright © 2019 Nikita_Dik. All rights reserved.
//

#include "ClientManager.hpp"

ClientManager::ClientManager(const SocketAddress& serverAddress)
{
    server.bind(serverAddress);
    server.listen(1); // Seems like a socket ignores my option and accepts more than one client
    
    acceptor = new SocketAcceptor<ClientHandler>(server, reactor);
    
    // Start the SocketReactor in another thread.
    // Reactor will notify SocketAcceptor when ServerSocket is ready to accept a new client
    serverThread.start(reactor);
}

ClientManager::~ClientManager()
{
    // Stop SocketReactor and close all ConnectionHandle's
    reactor.stop();
    
    // Wait for SocketReactor to be finished
    serverThread.join();
    cout << "[ClientManager] Reactor finished working.\n";
    
    // End accepting notifications from the stopped Reactor and free allocated memory
    delete acceptor;
}
