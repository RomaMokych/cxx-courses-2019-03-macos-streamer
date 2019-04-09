//
//  ServerWorker.cpp
//  SRD_Server
//
//  Created by Nikita on 4/9/19.
//  Copyright © 2019 Nikita. All rights reserved.
//

#include "ServerWorker.hpp"
#include "ScreenGrabber.hpp"


ServerWorker::ServerWorker(ServerSocket& Server, ScreenGrabber& Grabber) :
                                                        maxBufferSize(1024),
                                                        header_size(5),
                                                        server(Server),
                                                        grabber(Grabber),
                                                        client(nullptr)

{
    received_once = received_total = need_receive = current_packet_size = 0;
    messageID = -1;
    
    timeout = 10'000;
    finish = false;
    
    messageBuffer = new UInt8[maxBufferSize];
}


void ServerWorker::run()
{
    cout << "ServerWorker has been started\n";
    
    while(!finish)
    {
        AcceptClient();
        grabber.start();
        
        ReceiveData();
        grabber.stop();
    }
}

void ServerWorker::stop()
{
    cout << "ServerWorker has been stopped\n";
    
    finish = true;
    grabber.stop();
}

ServerWorker::~ServerWorker()
{
    cout << "ServerWorker has been destroyed\n";
    delete[] messageBuffer;
    
    if(client){
        client->close();
        delete client;
    }
    
}

void ServerWorker::AcceptClient()
{
    cout << "ServerWorker started accepting the clients\n";
    
    while(!finish)
    {

        if(server.poll(timeout, Socket::SELECT_READ)) // Server is able to accept
        {
            // Using pointer to future socket reusing
            client = new StreamSocket(server.acceptConnection());
            return;
        }
    }
        
     cout << "ServerWorker finished accepting the clients\n";
}

void ServerWorker::ReceiveData()
{
    cout << "ServerWorker started receiving data from the clients\n";

    while(!finish)
    {
        if(client->poll(timeout, Socket::SELECT_READ)) // Check if client has some data
        {
            received_once = client->receiveBytes(messageBuffer + received_total, maxBufferSize);
            
            if(received_once <= 0) // Connection is gracefully closed
            {
                client->close();
                delete client;
                client = nullptr;
                
                current_packet_size = received_total = received_once = 0;
                messageID = -1;
                
                return;
            }
            
            // Increment
            received_total += received_once;
            
            if(received_total < header_size) // In case when header hasn't arrived yet
            {
                continue;
            }
            
            // Otherwise, we can get size of the "packet"
            if(current_packet_size == 0){
                
                memcpy(&current_packet_size, messageBuffer, header_size );
                messageID = messageBuffer[header_size - 1];
                
                received_total -= header_size; // Maybe I shouldn't
            }
            
            if(received_total >= current_packet_size) // Received whole message
            {
                int x = 0, y = 0;
                char* data;
                
                switch(messageID) // Process message
                {
                    case MoveMouse :
                        
                        memcpy(&x, messageBuffer, 4);
                        memcpy(&y, messageBuffer+4, 4);
                        
                        // InputManager::MoveMouse(x,y);
                        cout << "InputManager::MoveMouse(" << x << "," << y << ")\n";
                        break;
                    
                    case 1 :
                        data = new char[current_packet_size];
                        memcpy(data, messageBuffer + header_size, current_packet_size);
                        
                        cout << data << "\n";
                        delete[] data;
                        break;
                        
                    default : break;
                }
                
                // Move tail to the head
                if(received_total > current_packet_size)
                {
                    memcpy(messageBuffer,
                           messageBuffer + current_packet_size,
                           received_total - current_packet_size);
                    
                    received_total = received_total - current_packet_size;
                    received_once = current_packet_size = 0;
                    messageID = -1;
                }
                
                received_total = received_once = current_packet_size = 0;
                messageID = -1;
                
                continue;
            }
            else if (received_total < current_packet_size)
            {
                continue;
            }
            
            return;
        }
    }
        
    cout << "ServerWorker stopped receiving data from the clients\n";
}
