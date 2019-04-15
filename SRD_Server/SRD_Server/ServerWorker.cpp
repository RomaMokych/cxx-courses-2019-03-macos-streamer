#include "InputManager.hpp"
#include "ServerWorker.hpp"
#include "ScreenGrabber.hpp"


ServerWorker:: ServerWorker(SocketAddress& address, shared_ptr<InputManager> inputManager) :
                                                        max_messageBuffer_size(1024*128), // 128 Kbytes
                                                        header_size(5),
                                                        serverSocket(address),
                                                        inManager(inputManager),
                                                        client(nullptr),
                                                        hasBytesToServe(false),
                                                        max_data_len(1024*64), // 64 Kbytes
                                                        max_screenFrameMessage_size(1920*1080*4)

{
    timeout = 10'000;
    finish = true;
    
    screenFrameMessage = new UInt8[max_screenFrameMessage_size];
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
        if(AcceptClient()){

            //grabber.start();
            
            ReceiveData();
            //grabber.stop();
        }
        else return;
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
        return false;
    
    cout << "Package size == " << package_size << " , message ID == " << messageID << "\n";
    
    return true;
}

void ServerWorker::processPayload(const u_long& msgID,
                                  const u_long& offset,
                                  const u_long& header_size,
                                  const u_long& current_packet_size)
{
    double x = 0, y = 0;
    char c;
    
    switch (msgID){
        case 1:
            inManager->press_LeftMouseButton(false);
            break;
        
        case 2:
            inManager->press_LeftMouseButton(true);
            break;
            
        case 3:
            inManager->press_RightMouseButton();
            break;
            
        case 4:
            inManager->hold_LeftMouseButton();
            break;
            
        case 5:
            memcpy(&x, &messageBuffer[offset + header_size], 8);
            memcpy(&y, &messageBuffer[offset + header_size + 8], 8);
            
            inManager->move_MouseDraggedTo(x, y);
            break;
            
        case 6:
            inManager->free_LeftMouseButton();
            break;
            
        case 7:
            memcpy(&x, &messageBuffer[offset + header_size], 8);
            memcpy(&y, &messageBuffer[offset + header_size + 8], 8);
            
            inManager->move_MouseTo(x, y);
            break;
            
        case 8:
            c = messageBuffer[offset + header_size];
            
            inManager->press_KeyboardChar(c);
            break;
        
        case 9:
            inManager->press_KeyTab(36);
            break;
            
        case 10:
            inManager->press_KeyTab(51);
            break;
            
        case 11:
            memcpy(&y, &messageBuffer[offset + header_size], 8);
            inManager->scroll(y);
            
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
//            inManager->move_MouseTo(x,y);
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
    
    u_long offset = 0, dropped = 0;
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
            
            // Connection is gracefully closed
            if(received_once <= 0)
            {
                this->DestroyClient();
                current_packet_size = received_total = received_once = 0;
                
                messageBuffer.resize(2048);
                
                cout << "Connection has been closed by the client\n\n";
                return;
            }
            
            // Increment
            received_total += received_once;
            hasBytesToServe = true;
            
            while(hasBytesToServe)
            {
                if(received_total < header_size) // Read more
                    break;
                
                if(current_packet_size == 0) // Haven't got package's data's length yet
                {
                    cout << "Haven't got package's data's length yet\n";
                    
                    if(!getPackageData(current_packet_size, messageID, offset))
                        return;
                }
                
                if(current_packet_size > max_data_len) // Too big packet, abort
                {
                    DestroyClient();
                    
                    messageBuffer.resize(2048);
                    
                    cout << "Unexpected-size package. Abort\n\n";
                    return;
                }
                
                u_long space_left = messageBuffer.size() - offset - header_size;
                
                // Not enough space for incoming package's data.
                if(current_packet_size > space_left)
                {
                    cout << "Not enough space for incoming package's data\n";
                
                        // There are some unused space before the offset
                        if(offset > 0)
                        {
                            cout << "There are some unused space before the offset. Let's use it instead of resizing buffer\n\n";
                            
                            // Move everything to the beggining of the buffer and restart
                            memcpy(&messageBuffer[0], &messageBuffer[0] + offset,
                                    messageBuffer.size() - offset);
                            
                            received_total = messageBuffer.size() - offset;
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
                        
                        // Need to read more data
                        hasBytesToServe = false;
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
                            u_long left = messageBuffer.size() - header_size - current_packet_size;
                            
                            if(left < header_size)
                            {
                                cout << "This piece of data even has no header, copy it to the beginning of the buffer\n";
                                
                                // If after this package even has no header
                                memcpy(&messageBuffer[0], &messageBuffer[0] + offset + header_size + current_packet_size, left);
                                received_total = offset = current_packet_size = 0;
                                
                                continue;
                                
                            }else{
                            
                                cout << "Move to the next package's header without any copying\n\n";
                                
                                // Move to the next package's header without any copying
                                offset = offset + header_size + current_packet_size;
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
void ServerWorker::sendFrame(UInt8* frame, int width, int height)
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
