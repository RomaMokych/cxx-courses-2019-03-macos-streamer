#include "InputManager.hpp"
#include "ServerWorker.hpp"
#include "ScreenGrabber.hpp"

ServerWorker:: ServerWorker(SocketAddress& address, shared_ptr<InputManager> inputManager) :
                                                        max_messageBuffer_size(1024*128), // 128 Kbytes
                                                        header_size(5),
                                                        serverSocket(address),
                                                        inputManager(inputManager),
                                                        client(nullptr),
                                                        max_data_len(1024*64), // 64 Kbytes
                                                        max_screenFrameMessage_size(1920*1080*4)

{
    timeout = 10'000;
    finish = true;
    
    packageReceiver = nullptr;
    serverSocket.setNoDelay(true);
}

void ServerWorker::setGrabber(shared_ptr<ScreenGrabber> grabber)
{
    if(!finish) // If already working
        return;
    
    this->screenGrabber = grabber;
}

void ServerWorker::run()
{
    cout << "ServerWorker has been started\n";
    finish = false;
    
    while(!finish)
    {
        if(AcceptClient())
        {
         // grabber->start();
            
          ReceiveData();
            
         // grabber->stop();
        }
        else return;
        
        if(packageReceiver)
            delete packageReceiver;
        
        packageReceiver = nullptr;
    }
}

void ServerWorker::stop()
{
    if(finish)
        return;
    
    cout << "ServerWorker has been stopped\n";
    
    finish = true;
}

ServerWorker::~ServerWorker()
{
    cout << "ServerWorker has been destroyed\n";
    DestroyClient();
    
    if(packageReceiver)
        delete packageReceiver;
    
    packageReceiver = nullptr;
}

bool ServerWorker::AcceptClient()
{
    cout << "ServerWorker started accepting the client\n";
    serverSocket.setNoDelay(true);
    
    while(!finish)
    {
        // Server is ready to accept the client
        if(serverSocket.poll(timeout, Socket::SELECT_READ))
        {
            // Using pointer for future socket reusing
            client = new StreamSocket(serverSocket.acceptConnection());
            
            // Set as big values as possible
            packageReceiver = new PackageReceiver(client,
                                                  2048*1024,
                                                  header_size,
                                                  1024*1024);
            
            cout << "Client has been accepted by the server\n";
            return true;
        }
    }
    return false;
}

void ServerWorker::DestroyClient()
{
     if(client){
        client->close();
        delete client;
        client = nullptr;
     }
}

void ServerWorker::sendData(UInt8* data, const u_long& length)
{
    cout << "Sending data mock\n";
    
    
}

void ServerWorker::processPayload(const u_long& msgID,
                                  const uint8_t* startOfPayload,
                                  const u_long& lengthOfPayload)
{
    double x = 0, y = 0;
    char character;
    
    switch (msgID){
        case ClickLeftMouseButton :
            inputManager->press_LeftMouseButton(false);
            break;
        
        case DoubleClickLeftMouseButton :
            inputManager->press_LeftMouseButton(true);
            break;
            
        case ClickRightMouseButton :
            inputManager->press_RightMouseButton();
            break;
            
        case HoldLeftMouseButton :
            //inputManager->hold_LeftMouseButton();
            break;
            
        // Haven't implemented well yet
        case MoveHeldMouse :

            break;
            
            if(lengthOfPayload < 16)
                break;
            
            memcpy(&x, startOfPayload, 8);
            memcpy(&y, startOfPayload + 8, 8);

            inputManager->move_MouseDraggedTo(x, y);
            break;
            
        case FreeLeftMouseButton :
            //inputManager->free_LeftMouseButton();
            break;
            
        case MoveMouseByVector :

            if(lengthOfPayload < 16)
                break;
            
            memcpy(&x, startOfPayload, 8);
            memcpy(&y, startOfPayload + 8, 8);
            
            inputManager->move_MouseTo(x, y);
            break;
            
        case PressKeyChar :
            int virtualKey, keyModification;
            memcpy(&virtualKey, startOfPayload, 4);
            memcpy(&keyModification, startOfPayload + 4, 4);
            
            inputManager->press_KeyTab(virtualKey, keyModification);
            break;
        
        case PressKeyTab :
            inputManager->press_KeyTab(36);
            break;
            
        case PressSpecialKeyTab :
            inputManager->press_KeyTab(51);
            break;
            
        case Scroll :
           // memcpy(&y, &messageBuffer[offset + header_size], 8);
            
            memcpy(&y,startOfPayload, 8);
            inputManager->scroll(y);
            
            break;

        default: break;
    }

}

void ServerWorker::ReceiveData()
{
    cout << "ServerWorker started receiving data from the client\n\n";

    while(!finish)
    {
        if(client->poll(timeout, Socket::SELECT_READ)) // Check if client has some data
        {
            // Read some data and then push it to the package processor(deFramer)

            int readResult = packageReceiver->readData();
            
            if(readResult == -1 || readResult == 0)  // Connection has been gracefully closed
            {
                cout << "Connection has been closed by the other side\n";
                DestroyClient();
                
                return;
            }
            else if(readResult < -1)  // Error occured
            {
                cout << "Read error\n";
                DestroyClient();
                
                return;
            }
            
            // Now we have some data. Process it (get msgID, length and payload), or read more bytes.
            
            int code = 0;
            
            do {
                
                code = packageReceiver->processPackage();
                
                if(code >= 0) // Process incoming request
                {
                    auto result = packageReceiver->getLastPayload();
                    processPayload(code, result.first, result.second); // code, buffer, size
                }
                else if(code == -1) // Not enough data
                {
                    cout << "Not enough data\n";
                    break; // Read more
                }
                else if (code == -2) // Continue
                {
                    
                    cout << "Continue package proccessing\n";
                    continue;
                }
                else if (code < -2) // Error occured
                {
                    cout << "Package proccessing error\n";
                    DestroyClient();
                    
                    return;
                }
            } while(packageReceiver->getHasBytesToServe() || !finish);
            
        }
    }
    cout << "ServerWorker stopped receiving data from the client\n";
    return;
}
 
