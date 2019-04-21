#include "Server.hpp"

using namespace MyClasses;

// Accepts frame in BGRA, then converts it and sends to the client
void Server::SendAll(UInt8* frame, int sizeInBytes){
    
    uint max_len = 4096;
    uint sent = 0; // Amount of TOTAL sent bytes
    
    cout << "Begin send..\n";
    
    // Declare "timer"
    // auto start = chrono::high_resolution_clock::now();
    // auto end = start;
    
       for(int i =0;i<sizeInBytes;i+=4)
       {
           std::swap(frame[i] , frame[i+3]);   // blue = alpha
           std::swap(frame[i+1] , frame[i+2]); // green = red
       }
    
    while(sent < sizeInBytes){
        
        sent += clientSocket.sendBytes(frame + sent, std::min(max_len, sizeInBytes - sent));
       // cout << sent << endl;
    }
    
    //end = chrono::high_resolution_clock::now();
    sent = 0;
    
    //auto duration = end - start;
    
   // cout << "Time elapsed : " << chrono::duration_cast<chrono::milliseconds>(duration).count() << " ms" << endl;
    
    }

void Server::BeginAccept(){
    while (true)
    {
        if (serverSocket.poll(timespan, Socket::SELECT_READ))
        {
            clientSocket = serverSocket.acceptConnection();
            
            if(clientSocket.address() != SocketAddress()){
                std::cout << "Connection accepted" << endl;
                break;
            }
        } 
    }
    }
