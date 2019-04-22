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
    
    this->grabber = grabber;
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
        if(serverSocket.poll(timeout, Socket::SELECT_READ)) // Server is able to accept
        {
            // Using pointer to future socket reusing
            client = new StreamSocket(serverSocket.acceptConnection());
            
            packageReceiver = new PackageReceiver(client,
                                                  8192,
                                                  header_size,
                                                  4096);
            
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

void sendData(UInt8*, const u_long&)
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
            inputManager->hold_LeftMouseButton();
            break;
            
        case MoveHeldMouse :
//            memcpy(&x, &messageBuffer[offset + header_size], 8);
//            memcpy(&y, &messageBuffer[offset + header_size + 8], 8);
            
            if(lengthOfPayload < 16)
                break;
            
            memcpy(&x, startOfPayload, 8);
            memcpy(&y, startOfPayload + 8, 8);

            inputManager->move_MouseDraggedTo(x, y);
            break;
            
        case FreeLeftMouseButton :
            inputManager->free_LeftMouseButton();
            break;
            
        case MoveMouseByVector :
//            memcpy(&x, &messageBuffer[offset + header_size], 8);
//            memcpy(&y, &messageBuffer[offset + header_size + 8], 8);
            
            if(lengthOfPayload < 16)
                break;
            
            memcpy(&x, startOfPayload, 8);
            memcpy(&y, startOfPayload + 8, 8);
            
            inputManager->move_MouseTo(x, y);
            break;
            
        case PressKeyChar :
            character = *startOfPayload;
            
            inputManager->press_KeyboardChar(character);
            break;
        
        case PressKeyTab : inputManager->press_KeyTab(36);
            break;
            
        case PressSpecialKeyTab : inputManager->press_KeyTab(51);
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
            } while(packageReceiver->getHasBytesToServe());
            
        }
    }
    cout << "ServerWorker stopped receiving data from the client\n";
    return;
}
            /*
             
            // --------------------------------------------------------//
            // Read some data and then push it to the package processor
            // --------------------------------------------------------//
             
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
             
             // Now we have some data. Let's process it
             
             int code = 0;
             
             do {
             
                 code = packageReceiver->processPackage();
             
                 if(code >= 0) // Process incoming request
                 {
                    auto result = packageReceiver->getLastPayload();
                    processPayload(code, result.first, result.second); // code, size, buffer
                 }
                 else if(code == -1) // Not enough data
                 {
                    break; // Read more
                 }
                 else if (code == -2) // Continue
                 {
                    cout << "Continue package proccessing\n";
                    continue();
                 }
                 else if (code < -2) // Error occured
                 {
                     cout << "Package proccessing error\n";
                     DestroyClient();
             
                     return;
                 }
             } while(packageReceiver->hasBytesToServe());
             
        }
    }
     cout << "ServerWorker stopped receiving data from the client\n";
     return;
 }
             //
             
             
             bool PackageReceiver::CheckAndResizeBuffer()
             {
             
             // Not enough space to read more data
             if(messageBuffer.size() - received_total - offset <= 0 ) {
             
                 cout << "Not enough space to read more data\n";
             
                 unsigned long available_to_resize = max_messageBuffer_size - messageBuffer.size();
             
                 if(available_to_resize <= 0) return false; // Can't allocate more memory
                 else messageBuffer.resize(messageBuffer.size() + available_to_resize * 0.15);
             
                 cout << "Space has been allocated\n";
             
                }
                return true;
             }
            
            
             int PackageReceiver::readData()
             {
                if(!client)
                    return -2;
             
                 // Resizes only if there is no other way to fix the space problem
                 if(!CheckAndResizeBuffer(received_total, offset))
                    return -2;
             
                 // Read as big piece of data as possible
                 u_long start_pos = received_total + offset;
                 u_long max_piece_size = messageBuffer.size() - received_total - offset;
             
                 received_once = client->receiveBytes(&messageBuffer[0] + start_pos, max_piece_size);
             
                 cout << "\nReceived once : " << received_once << "\n";
             
                 if(received_once <= 0)  // Connection is gracefully closed or error occured
                 {
                     current_packet_size = received_total = received_once = 0;
                     messageBuffer.resize(max_messageBuffer_size * 0.2); // Free some memory
             
                     return received_once; // Return error code
                 }
             
                 // Otherwise
             
                 received_total += received_once;
                 hasBytesToServe = true;
             
                 return received_once;
             }
             
             bool PackageReceiver::hasBytesToServe() const { return hasBytesToServe; }
             
             int PackageReceiver::processPackage()
             {
                if(!hasBytesToServe)
                    return -1; // Doesn't have any data to interact with
             
                 cout << "Has bytes to serve\n";
             
                 hasBytesToServer = false; // Drop flag until we finish
         
                 if(received_total < header_size) // Not enough data
                 {
                     u_long left = messageBuffer.size() - header_size - current_packet_size - offset;
             
                    // If this is the end of the buffer and last package even has no header
                     if(left < header_size) {
                         cout << "Last piece of data even has no header, copy it to the beginning of the buffer\n";
         
                         memcpy(&messageBuffer[0], &messageBuffer[0] + offset + header_size + current_packet_size, left);
                         offset = current_packet_size = 0;
             
                         received_total = left;
                    }
                     // In both cases
                     return -1; // Read more data before we can continue
                  }
             
                 if(current_packet_size == 0) // Haven't got package's data length yet
                 {
                    cout << "Haven't got package's data length yet\n";
             
                    if(!getPackageData(current_packet_size, messageID, offset))
                        return -3; // Error
                 }
             
                 if(current_packet_size > max_data_len) // Too big packet, abort
                 {
                    messageBuffer.resize(max_messageBuffer_size * 0.2);
             
                    cout << "Unexpected-size package. Abort\n\n";
                    return -3;
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
             
                         received_total = space_left + header_size; // Reset it like we've read only those bytes
                         offset = 0;
             
                         return -1; // Read more
             
                         }else{
                         cout << "Resizing and restarting..\n\n";
             
                         messageBuffer.resize(messageBuffer.size() + current_packet_size);
                         return -1; // Read more
                     }
                 }
             
                 // Current amount of user data
                 u_long amout_of_read_data = received_total - header_size - offset;
             
                 if(current_packet_size > amout_of_read_data)
                 {
                     cout << "Need to read more data\n\n";
             
                     hasBytesToServe = false;
                     return -1;
                 }
             
                 if(amout_of_read_data >= current_packet_size) // Now we have enough bytes
                 {
                 // this->processPayload(messageID, offset, header_size, current_packet_size);
                 // return messageID;
             
                 if(amout_of_read_data == current_packet_size)
                 {
                     cout << "Have read whole package and nothing left. Start writing to the beggining of the buffer\n\n";
             
                     // Save last userdata pointers to be returned for user
                    saveLastPackageInfo();
             
                     // Then start from the empty buffer
                     received_total = current_packet_size = offset = 0;
                     hasBytesToServe = false;
             
                     return messageID; // return code of ready package
                     //break;
                 }
                 else if (amout_of_read_data > current_packet_size) // What if we have more than one package in our buffer?
                 {
             
                 // How many data left behind this package?
                 u_long left = messageBuffer.size() - header_size - current_packet_size - offset;
             
                 if(left < header_size)
                 {
                     cout << "This piece of data even has no header, prepare it to copy to the beginning of the buffer\n";
             
                         //memcpy(&messageBuffer[0], &messageBuffer[0] + offset + header_size + current_packet_size, left);
                         //offset = current_packet_size = 0;
             
                     // If this package even has no header, prepare it to be stored at the beginning of the buffer
                     received_total = left;
             
                     hasBytesToServer = true;
                     return -2; // Continue
             
                 }else{
             
                     cout << "Move to the next package's header without any copying\n\n";
             
                    // Save last userdata pointers to be returned for user
                    saveLastPackageInfo();
             
                     // Move to the next package's header without any copying
                     offset += (header_size + current_packet_size);
                     current_packet_size = 0;
             
                    // Mark: Added received_total correction, now it contains size of really useful infomation(even with header data)
                     received_total -= (header_size + current_packet_size); // Left without current package
             
                     hasBytesToServer = true;
                     return -2; // Continue
             
                     //continue;
                     }
                 }
                 }
             
             
              }
             
             // Saves last userdata pointers to be returned for user
             // CAUTION : ANY RETURNED USERDATA USING THIS POINTERS WILL BE OVERWITTEN AFTER NEXT CALL TO 'read()' !
             void saveLastPackageInfo()
             {
                 lastStart =  &messageBuffer[offset + header_size];
                 lastLenght = current_packet_length;
             }
             
             private:
                const StreamSocket* const client; // Can check for nullptr
                std::vector<UInt8> messageBuffer;
             
                int current_packet_size, received_total, received_once;
                u_long offset;
            
            */
            /*
            // Resizes only if there is no other way to fix the space problem
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
                
                if(received_total < header_size) // Not enough data. Read more
                {
                    // No more space to store  header , but has some data at the end
                    u_long left = messageBuffer.size() - header_size - current_packet_size - offset;
                    
                    if(left < header_size)
                    {
                        cout << "Last piece of data even has no header, copy it to the beginning of the buffer\n";
                        
                        // If this package even has no header
                        memcpy(&messageBuffer[0], &messageBuffer[0] + offset + header_size + current_packet_size, left);
                        offset = current_packet_size = 0;
                        received_total = left;
                        
                        continue;
                    }
                    
                    break;   // If there is enough space to continue reading
                }
                
                if(current_packet_size == 0) // Haven't got package's data's length yet
                {
                    cout << "Haven't got package's data's length yet\n";
                    
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
                        
                        // return -1;
                    }
                
                    if(amout_of_read_data >= current_packet_size) // Now we have enough bytes
                    {
                        this->processPayload(messageID, offset, header_size, current_packet_size);
                        // return messageID;
                        
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
void ServerWorker::sendData(UInt8* data, const u_long& size)
{
    if(!data)
        return;
    
//
//    UInt8 type = 2;
//
//    // Allows to send raw images
//    memcpy(screenFrameMessage, &size, 4); // Lenght of message
//    memcpy(screenFrameMessage + 4, &type, 1);  // Type of message
//    memcpy(screenFrameMessage + 4 + 1, &width, 4);
//    memcpy(screenFrameMessage + 4 + 1 + 4, &height, 4);
//
//    memcpy(screenFrameMessage + header_size, frame, width*height);
//
//    for(int i =0;i<width*height*4;i+=4)
//    {
//        std::swap(frame[i] , frame[i+3]);   // blue = alpha
//        std::swap(frame[i+1] , frame[i+2]); // green = red
//    }
    
    //client->sendBytes(frame, width*height*4);
}
*/
