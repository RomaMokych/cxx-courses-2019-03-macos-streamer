#include "SRD_Server.hpp"
#include "ScreenGrabber.hpp"
#include "ServerWorker.hpp"

SRD_Server::SRD_Server(const SocketAddress& Address) :
                                    address(Address),
                                    serverSocket(nullptr),
                                    screenGrabber(nullptr),
                                    serverWorker(nullptr),
                                    workerThread(nullptr),
                                    working(false),
                                    max_screenFrameMessage_size(1920*1080*4),
                                    header_size(5)
{
    cout << "Server has been created\n";
    
    screenFrameMessage = new UInt8[max_screenFrameMessage_size];
}

SRD_Server::~SRD_Server()
{
    this->stop();
    delete[] screenFrameMessage;
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

// Work in progress. Now sends raw image data, but it's extemely inefficient for even local wireless network
void SRD_Server::sendFrame(UInt8* frame, int width, int height)
{
    if(!frame)
        return;
    
    int totalSize = width*height;
    UInt8 type = 2;
    
    // Allows to send raw images
    memcpy(screenFrameMessage, &totalSize, 4); // Lenght of message
    memcpy(screenFrameMessage + 4, &type, 1);  // Type of message
    memcpy(screenFrameMessage + 4 + 1, &width, 4);
    memcpy(screenFrameMessage + 4 + 1 + 4, &height, 4);

    memcpy(screenFrameMessage + header_size, frame, width*height);
 
    StreamSocket* client = serverWorker->getClient();
    
    for(int i =0;i<width*height*4;i+=4)
    {
        std::swap(frame[i] , frame[i+3]);   // blue = alpha
        std::swap(frame[i+1] , frame[i+2]); // green = red
    }
    
    //client->sendBytes(frame, width*height*4);
}
