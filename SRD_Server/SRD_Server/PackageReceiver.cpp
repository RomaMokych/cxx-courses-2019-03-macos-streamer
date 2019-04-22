#include "PackageReceiver.hpp"

PackageReceiver::PackageReceiver(StreamSocket* _client,
                                 const u_long& _max_messageBuffer_size,
                                 const u_long& _header_size,
                                 const u_long& _max_data_len) :
    client(_client),
    max_messageBuffer_size(_max_messageBuffer_size),
    header_size(_header_size),
    max_data_len(_max_data_len)
{
    hasBytesToServe = false;
    current_packet_size = received_total = offset = lastLenght = messageID = received_once = 0;
    
    lastStartPos = nullptr;
}

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
    if(!CheckAndResizeBuffer())
        return -2;
    
    // Read as big piece of data as possible
    u_long start_pos = received_total + offset;
    u_long max_piece_size = messageBuffer.size() - received_total - offset;
    
    cout << "start_pos : " << start_pos << "\n";
    cout << "max_piece_size : " << max_piece_size << "\n";
    
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
    cout << "received_total += received_once : " << received_total << "\n";
    
    hasBytesToServe = true;
    
    return received_once;
    
}


bool PackageReceiver::getPackageData()
{
    memcpy(&current_packet_size, &messageBuffer[offset], 4);
    messageID = messageBuffer[offset + 4];
    
    cout << "Package size == " << current_packet_size << " , message ID == " << messageID << "\n";
    
    if(current_packet_size <= 0 || messageID < 0)
    {
        cout << "Package error\n";
        return false;
    }
    
    return true;
}


int PackageReceiver::processPackage()
{
    if(!hasBytesToServe)
        return -1; // Doesn't have any data to interact with
    
    cout << "Has bytes to serve\n";
    
    hasBytesToServe = false; // Drop flag until we finish
    
    // 1. We can read more
    // 2. We can resize and read more
    // 3. We can move remaining part to the beginning and read more without any resizing
    if(received_total < header_size) // Not enough data
    {
        cout << "Not enough data (received_total < header_size)" << "\n";
       // u_long left = messageBuffer.size() - header_size - current_packet_size - offset;
      
       // u_long left = received_total - header_size - current_packet_size - offset;
        
        // BUG -> received <<<< header_size
        //u_long left = received_total - header_size - current_packet_size - offset;
        
        // Amount of remaining data
        u_long left = messageBuffer.size() - offset;
        
        cout << "Amount of remaining data : " << left << "\n";
        
        // If this is the end of the buffer and last package even has no header + there is no space for new header
        
        cout << "header_size - received_total : " << header_size - received_total << "\n";
        cout << (left < (header_size - received_total) ? "left < header_size - rceived" : "left > header_size - received_total ") << "\n";
        
        if(left < header_size - received_total) { // rec < header
            cout << "Last piece (received_total) of data at the end of the buffer even has no header, copy it to the beginning of the buffer\n";
            
            memcpy(&messageBuffer[0], &messageBuffer[0] + offset + header_size + current_packet_size, left);
            
        
            offset = current_packet_size = 0;
            cout << "Offset : " << offset << "\n";
            
           // received_total = left;
        }
        // In both cases
        return -1; // Read more data before we can continue
    }
    
    if(current_packet_size == 0) // Haven't got package's data length yet
    {
        cout << "Haven't got package's data length yet\n";
        
        if(!getPackageData())
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
    // OVERFLOW BUG>?????
   // u_long amout_of_read_data = received_total - header_size - offset;
    
    // received_total >= header_size, see at the top!
    u_long amout_of_read_data = received_total - header_size;
    
     cout << "Amount of read data : " << amout_of_read_data << "\n";
    
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
        
        // Save last userdata pointers to be returned for user
        saveLastPackageInfo();
        
        if(amout_of_read_data == current_packet_size)
        {
            cout << "Have read whole package and nothing left. Start writing to the beggining of the buffer\n\n";
            
            // Then start from the empty buffer
            received_total = current_packet_size = offset = 0;
            hasBytesToServe = false;
            
            return messageID; // return code of ready package
            //break;
        }
        else if (amout_of_read_data > current_packet_size) // What if we have more than one package in our buffer?
        {
            
            // How many data left behind this package?
           // u_long left = messageBuffer.size() - header_size - current_packet_size - offset;
           // u_long left = received_total - header_size - current_packet_size - offset;
            
            // received_total > hedaer_size, see higher!
            //  current_packet_size < received_total !!
            u_long left = received_total - header_size - current_packet_size;
            
            if(left < header_size)
            {
                cout << "This piece of data even has no header, prepare it to copy to the beginning of the buffer\n";
                
                //memcpy(&messageBuffer[0], &messageBuffer[0] + offset + header_size + current_packet_size, left);
                //offset = current_packet_size = 0;
                
                // If this package even has no header, prepare it to be stored at the beginning of the buffer
                offset += header_size + current_packet_size; // !!!
                received_total = left;
                
                 cout << "offset : " << offset << "\n";
                 cout << "received_total : " << left << "\n";
                
                hasBytesToServe = true;
                return messageID; // return code of ready package
                
            }else{
                
                cout << "Move to the next package's header without any copying\n\n";
                
                // Move to the next package's header without any copying
                offset += (header_size + current_packet_size);
               
                
                // Mark: Added received_total correction, now it contains size of really useful infomation(even with header data)
                received_total -= (header_size + current_packet_size); // Left without current package
                
                cout << "offset : " << offset << "\n";
                cout << "received_total : " << left << "\n";
                
                current_packet_size = 0;
                
                hasBytesToServe = true;
                return messageID; // return code of ready package
                
                //continue;
            }
        }
    }
    return -3; // Default
}

// Saves last userdata pointers to be returned for user
// CAUTION : ANY RETURNED USERDATA USING THIS POINTERS WILL BE OVERWITTEN AFTER NEXT CALL TO 'read()' !
void PackageReceiver::saveLastPackageInfo()
{
    lastStartPos = &messageBuffer[offset + header_size];
    lastLenght = current_packet_size;
}

std::pair<uint8_t*, u_long> PackageReceiver::getLastPayload()
{
    return std::make_pair(lastStartPos, lastLenght);
}
