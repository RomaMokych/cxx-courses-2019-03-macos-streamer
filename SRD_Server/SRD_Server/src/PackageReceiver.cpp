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
    if(messageBuffer.size() - received_total <= 0 ) {
        
        cout << "Not enough space to read more data\n";
        
        unsigned long available_to_resize = max_messageBuffer_size - messageBuffer.size();
        
        if(available_to_resize <= 0) return false; // Can't allocate more memory
        else messageBuffer.resize(messageBuffer.size() + available_to_resize * 0.1);
        
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
    u_long start_pos = received_total;
    u_long max_piece_size = messageBuffer.size() - received_total;
    
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
    memcpy(&current_packet_size, &messageBuffer[offset], 4); // 4 bytes from beginning of actual data
    messageID = messageBuffer[offset + 4]; // One more byte
    
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
    
    u_long actual_data = received_total - offset;
    
    if(actual_data < header_size) // Not enough data
    {
        // In this case, actual_data is a number of bytes that less than header_size and there are no bytes after it

        cout << "Not enough data (actual_data < header_size)" << "\n";
        
        if(actual_data == 0) // Special case, ask for more bytes immedeatly
            return -1;

        // Buffer has some unused space before the 'offset' and at least our data can fit in this area
        if(offset >= actual_data)
        {
             // Move actual_data to the beginning of the buffer and move "pointers" left by 'offset' bytes
             memcpy(&messageBuffer[0], &messageBuffer[0] + offset, actual_data);
             received_total -= offset;
             offset = 0; // offset -= offset;
            
             cout << "Move actual_data to the beginning\n";
        }

        // In both cases, read more data before we can continue
        return -1;
    }
    
    // At this point we have more or equal to 'header_size' bytes
    
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
    
    // Not enough space for incoming package's data
    if(current_packet_size > space_left)
    {
        cout << "Not enough space for incoming package's data\n";
        
        // If there are some unused space before the 'offset' which we can use to store bytes
        if(offset > 0)
        {
            cout << "There are some unused space before the offset. Let's use it instead of resizing buffer\n\n";
            
            // Move everything to the beggining of the buffer and restart
            memcpy(&messageBuffer[0], &messageBuffer[0] + offset, messageBuffer.size() - offset);
            
            received_total -= offset;
            offset = 0; // offset -= offset;
            
            hasBytesToServe = false;
            return -1; // Read more
            
        }else{
            cout << "Resizing and restarting..\n\n";
            
            messageBuffer.resize(messageBuffer.size() + current_packet_size - space_left);
            
            hasBytesToServe = false;
            return -1; // Read more
        }
    }
    
    
    u_long available_data = received_total - header_size - offset;
    
    if(current_packet_size > available_data)
    {
        cout << "Need to read more data\n\n";
        
        hasBytesToServe = false;
        return -1;
    }
    
    if(available_data >= current_packet_size) // Now we have enough bytes
    {
        // Save last userdata pointers to be returned for user
        saveLastPackageInfo();
        
        if(available_data == current_packet_size)
        {
            cout << "Have read whole package and nothing left. Start writing to the beggining of the buffer\n\n";
            
            // Then start from the empty buffer
            received_total = current_packet_size = offset = 0;
            hasBytesToServe = false;
            
            return messageID; // return code of ready package
        }
        // What if we have more than one package in our buffer?
        else if (available_data > current_packet_size)
        {
            // OK, how many data left behind this package?
            u_long left = received_total - header_size - current_packet_size;
            
            if(left < header_size)
            {
                cout << "This piece of data even has no header, but check it in the next iteration\n";
            
                cout << "offset : " << offset << "\n";
                cout << "received_total : " << left << "\n";
                
                hasBytesToServe = true;
                return messageID; // return code of ready package
                
            }else{
                
                cout << "Move to the next package's header without any copying\n\n";
                
                offset += header_size + current_packet_size; // Move to the right by one package
                
                hasBytesToServe = true;
                return messageID; // return code of ready package
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
