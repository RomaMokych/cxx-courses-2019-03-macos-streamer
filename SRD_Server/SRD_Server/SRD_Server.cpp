//
//  SRD_Server.cpp
//  SRD_Server
//
//  Created by Nikita on 4/8/19.
//  Copyright © 2019 Nikita. All rights reserved.
//

#include "SRD_Server.hpp"
#include "ScreenGrabber.hpp"
#include "ServerWorker.hpp"

SRD_Server::SRD_Server(const SocketAddress& Address) :
                                    address(Address),
                                    serverSocket(nullptr),
                                    screenGrabber(nullptr),
                                    serverWorker(nullptr),
                                    workerThread(nullptr),
                                    working(false)
{
    cout << "Server has been created\n";
}

SRD_Server::~SRD_Server()
{
    this->stop();
}

bool SRD_Server::start()
{
    if(working) // Nothing to do
        return false;
    
    serverSocket = new ServerSocket(address);
    screenGrabber = new ScreenGrabber(*this);
    serverWorker = new ServerWorker(*serverSocket, *screenGrabber);
    
    cout << "Server has been started\n";
    
    workerThread = new Thread;
    workerThread->start(*serverWorker);
    
    working = true;
    
    return true;
}

bool SRD_Server::stop()
{
    if(!working) // Nothing to do
        return false;
    
    serverWorker->stop();
    workerThread->join();
    
    delete workerThread;
    delete serverWorker;
    
    delete screenGrabber;
    serverSocket->close();
    
    delete serverSocket;
    
    working = false;
    
    return true;
}
