//
//  ClientAcceptor.hpp
//  RDP_Server
//
//  Created by Nikita on 4/8/19.
//  Copyright © 2019 Nikita_Dik. All rights reserved.
//

#ifndef ClientAcceptor_hpp
#define ClientAcceptor_hpp

#include "RegularHeaders.h"
#include "NetworkHeaders.h"
#include "ClientHandler.hpp"

class ClientAcceptor
{
public:

    ClientAcceptor(ServerSocket& Server, SocketReactor& Reactor, size_t MaxConnections) :
                                                        server(Server), reactor(&Reactor),
                                                        clients_count(0),
                                                        max_connections(MaxConnections),
                                                        onClientAcceptDone(true)
    {
        clients_count = new std::atomic<size_t>(0);
        reactor->addEventHandler(server, Observer<ClientAcceptor, ReadableNotification>(*this, &ClientAcceptor::onClientAccept));
        cout << "\n";
    }

    void onClientAccept(ReadableNotification* pNotification)
    {
        onClientAcceptDone = false;
        pNotification->release();

        StreamSocket sock = server.acceptConnection();
        
        if(*clients_count >= max_connections){
            sock.close();
            
            cout << "[Server] Server is full. Connection rejected.\n";
            onClientAcceptDone = true;
            return;
        }
        
        ++*clients_count;

        reactor->wakeUp();
        new ClientHandler(sock, *reactor, clients_count); // Destructs itself later

        onClientAcceptDone = true;
    }

    ~ClientAcceptor()
    {
        reactor->removeEventHandler(server, Observer<ClientAcceptor, ReadableNotification>(*this, &ClientAcceptor::onClientAccept));
        while(!onClientAcceptDone); // Waiting for all proccesses to be finished before closing
        
        delete clients_count;
    }

private :
    ServerSocket   server;
    SocketReactor* reactor;
    
    std::atomic<size_t>* clients_count;
    size_t max_connections;

    bool onClientAcceptDone;
};

#endif /* ClientAcceptor_hpp */
