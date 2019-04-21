#include "InputManager.hpp"
#include "ServerWorker.hpp"
#include "ScreenGrabber.hpp"

ServerWorker:: ServerWorker(SocketAddress& address, shared_ptr<InputManager> inputManager) :
                                                        max_messageBuffer_size(1024*128), // 128 Kbytes
                                                        header_size(5),
                                                        serverSocket(address),
                                                        inputManager(inputManager),
                                                        client(nullptr),
                                                        hasBytesToServe(false),
                                                        max_data_len(1024*64), // 64 Kbytes
                                                        max_screenFrameMessage_size(1920*1080*4)

{
    timeout = 10'000;
    finish = true;
    
    screenFrameMessage = new UInt8[max_screenFrameMessage_size];
    serverSocket.setNoDelay(true);
}


void ServerWorker::setGrabber(shared_ptr<ScreenGrabber> grabber)
{
    if(!finish) // If already working
        return;
    
    this->grabber = grabber;
}

void ServerWorker::run()
{
    cout << "ServerWorker has been started\n";
    finish = false;
    
    while(!finish)
    {
        grabber->start();
        if(AcceptClient()){

            //grabber.start();
            
            ReceiveData();
            //grabber.stop();
        }
        else return;
        grabber->start();
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
    delete[] screenFrameMessage;
}

bool ServerWorker::AcceptClient()
{
    cout << "ServerWorker started accepting the client\n";
    serverSocket.setNoDelay(true);
    
    while(!finish)
    {
        if(serverSocket.poll(timeout, Socket::SELECT_READ)) // Server is able to accept
        {
            // Using pointer to future socket reusing
            client = new StreamSocket(serverSocket.acceptConnection());
            
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

bool ServerWorker::CheckAndResizeBuffer(const u_long& received_total, const u_long& offset)
{
    // Not enough space to read more data
    if(messageBuffer.size() - received_total - offset <= 0 ){
        
        cout << "Not enough space to read more data\n";
        
        unsigned long available_to_resize = max_messageBuffer_size - messageBuffer.size();
        
        if(available_to_resize <= 0) return false; // Can't allocate more memory
        else messageBuffer.resize(messageBuffer.size() + available_to_resize * 0.15);
        
        cout << "Space has been allocated\n";
    }
    return true;
}

bool ServerWorker::getPackageData(u_long& package_size, int& messageID, const u_long& offset)
{
    memcpy(&package_size, &messageBuffer[offset], 4);
    messageID = messageBuffer[offset + 4];
    
    if(package_size <= 0 || messageID < 0)
    {
        cout << "Package size == " << package_size << " , message ID == " << messageID << "\n";
        return false;
    }
    
    return true;
}

void ServerWorker::processPayload(const u_long& msgID,
                                  const u_long& offset,
                                  const u_long& header_size,
                                  const u_long& current_packet_size)
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
            inputManager->hold_LeftMouseButton();
            break;
            
        case MoveHeldMouse :
            memcpy(&x, &messageBuffer[offset + header_size], 8);
            memcpy(&y, &messageBuffer[offset + header_size + 8], 8);
            
            inputManager->move_MouseDraggedTo(x, y);
            break;
            
        case FreeLeftMouseButton :
            inputManager->free_LeftMouseButton();
            break;
            
        case MoveMouseByVector :
            memcpy(&x, &messageBuffer[offset + header_size], 8);
            memcpy(&y, &messageBuffer[offset + header_size + 8], 8);
            
            inputManager->move_MouseTo(x, y);
            break;
            
        case PressKeyChar :
            character = messageBuffer[offset + header_size];
            
            inputManager->press_KeyboardChar(character);
            break;
        
        case PressKeyTab : inputManager->press_KeyTab(36);
            break;
            
        case PressSpecialKeyTab : inputManager->press_KeyTab(51);
            break;
            
        case Scroll :
            memcpy(&y, &messageBuffer[offset + header_size], 8);
            inputManager->scroll(y);
            
            break;

        default: break;
    }
//        case MoveMouse :
//        {
//            double x = 0, y = 0;
//
//            memcpy(&x, &messageBuffer[offset + header_size], 8);
//            memcpy(&y, &messageBuffer[offset + header_size + 8], 8);
//
//            inputManager->move_MouseTo(x,y);
//
//            break;
//        }
//
//        case TextMessage :
//        {
//            char* data = new char[current_packet_size];
//            memcpy(data, &messageBuffer[offset + header_size], current_packet_size);
//
//            cout << "Arrived text message, size == " << current_packet_size << "\n";
//
//            cout << data << "\n";
//            delete[] data;
//            break;
//        }

}

void ServerWorker::ReceiveData()
{
    cout << "ServerWorker started receiving data from the client\n\n";
    
    u_long offset = 0;
    u_long received_once = 0, received_total = 0;
    u_long current_packet_size = 0;
    
    int messageID = -1;
    
    while(!finish)
    {
        if(client->poll(timeout, Socket::SELECT_READ)) // Check if client has some data
        {
            if(!this->CheckAndResizeBuffer(received_total, offset))
                return;
         
            // Read as big piece of data as possible
            u_long start_pos = received_total + offset;
            u_long max_piece_size = messageBuffer.size() - received_total - offset;
            
            received_once = client->receiveBytes(&messageBuffer[0] + start_pos, max_piece_size);
            
            cout << "\nReceived once : " << received_once << "\n";
            
            if(received_once <= 0)  // Connection is gracefully closed
            {
                this->DestroyClient();
                current_packet_size = received_total = received_once = 0;
                
                messageBuffer.resize(max_messageBuffer_size * 0.2);
                
                cout << "Connection has been closed by the client\n\n";
                return;
            }
            
            received_total += received_once;  // Increment
            hasBytesToServe = true;
            
            while(hasBytesToServe)
            {
                 cout << "Has bytes to serve\n";
                
                if(received_total < header_size) // Read more
                    break;
                
                if(current_packet_size == 0) // Haven't got package's data's length yet
                {
                    cout << "Haven't got package's data's length yet\n";
                    
                    // Fail when sending scroll
                    if(!getPackageData(current_packet_size, messageID, offset))
                        return;
                }
                
                if(current_packet_size > max_data_len) // Too big packet, abort
                {
                    DestroyClient();
                    
                    messageBuffer.resize(max_messageBuffer_size * 0.2);
                    
                    cout << "Unexpected-size package. Abort\n\n";
                    return;
                }
                
                u_long space_left = messageBuffer.size() - offset - header_size;
                
                // Not enough space for incoming package's data.
                if(current_packet_size > space_left)
                {
                    cout << "Not enough space for incoming package's data\n";
                
                        if(offset > 0) // If there are some unused space before the offset
                        {
                            cout << "There are some unused space before the offset. Let's use it instead of resizing buffer\n\n";
                            
                            // Move everything to the beggining of the buffer and restart
                            memcpy(&messageBuffer[0], &messageBuffer[0] + offset, messageBuffer.size() - offset);
                            
                            received_total = space_left + header_size;
                            offset = 0;
                            continue;
                            
                        }else{
                            cout << "Resizing and restarting..\n\n";
                            
                            messageBuffer.resize(messageBuffer.size() + current_packet_size);
                            continue;
                        }
                    }
                    
                    u_long amout_of_read_data = received_total - header_size - offset;
                    
                    if(amout_of_read_data < current_packet_size)
                    {
                        cout << "Need to read more data\n\n";
                        
                        hasBytesToServe = false;  // Need to read more data
                        break;
                    }
                
                    if(amout_of_read_data >= current_packet_size) // Now we have enough bytes
                    {
                        this->processPayload(messageID, offset, header_size, current_packet_size);
                        
                        if(amout_of_read_data == current_packet_size)
                        {
                            cout << "Have read whole package and nothing left. Start writing to the beggining of the buffer\n\n";
                            
                            // Start from the empty buffer
                            received_total = current_packet_size = offset = 0;
                            break;
                        }
                        else if (amout_of_read_data > current_packet_size)
                        {
                            u_long left = messageBuffer.size() - header_size - current_packet_size - offset;
                            
                            if(left < header_size)
                            {
                                cout << "This piece of data even has no header, copy it to the beginning of the buffer\n";
                                
                                // If this package even has no header
                                memcpy(&messageBuffer[0], &messageBuffer[0] + offset + header_size + current_packet_size, left);
                                offset = current_packet_size = 0;
                                received_total = left;
                                
                                continue;
                                
                            }else{
                            
                                cout << "Move to the next package's header without any copying\n\n";
                                
                                // Move to the next package's header without any copying
                                offset += (header_size + current_packet_size);
                                current_packet_size = 0;
                                
                                continue;
                            }
                        }
                    }
            }
        }
    }
    cout << "ServerWorker stopped receiving data from the client\n";
    return;
}

// Work in progress. Now sends raw image data, but it's extemely inefficient for even local wireless network
void ServerWorker::sendFrame(UInt8* frame, u_long width, u_long height)
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
    
    for(int i =0;i<width*height*4;i+=4)
    {
        std::swap(frame[i] , frame[i+3]);   // blue = alpha
        std::swap(frame[i+1] , frame[i+2]); // green = red
    }
    
    //client->sendBytes(frame, width*height*4);
}
