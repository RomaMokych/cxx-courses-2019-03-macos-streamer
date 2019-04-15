#include "SRD_Server.hpp"
#include "ScreenGrabber.hpp"
#include "ServerWorker.hpp"

SRD_Server::SRD_Server(shared_ptr<ServerWorker> worker) :
                                    serverWorker(worker),
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
    
    working = false;
    
    return true;
}

