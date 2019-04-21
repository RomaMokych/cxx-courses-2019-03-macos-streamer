#include "Server.hpp"

using namespace MyClasses;

// Accepts frame in BGRA, then converts it and sends to the client
void Server::SendAll(UInt8* frame, int sizeInBytes){
    
    uint max_len = 4096;
    uint sent = 0; // Amount of TOTAL sent bytes
    
    cout << "[Server]Begin send..\n";
    
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
    cout << "[Server]Sent successfully!\n";
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
                std::cout << "[Server]Connection accepted" << endl;
                break;
            }
        } 
    }
    
    }

void Server::Start()
{
    const char* ipAddress = "localhost";
    serverSocket.bind(SocketAddress(ipAddress, 9999));
    serverSocket.listen();
}

void Server::BeginReceive()
{
    while (true)
    {
        if (clientSocket.poll(timespan, Socket::SELECT_READ))
        {
            clientSocket.receiveBytes(commandBuffer, 18);
            std::cout << "[Server]Received input from client : \n";
            double x = 0, y = 0;
            
            uint8_t xRep[8], yRep[8];
            
            for(int i =0;i<8;i++){
                xRep[i] = commandBuffer[i+1];
                yRep[i] = commandBuffer[i+1+8];
            }

            if(commandBuffer[0] == commandBuffer[17] == 1)
            {

                memcpy(&x, xRep, sizeof(double));
                memcpy(&y, yRep, sizeof(double));
                
                 //get mouse location
                 CGEventRef event = CGEventCreate(NULL);
                 CGPoint point = CGEventGetLocation(event);
                 std::cout << " [Old pos] x: " << point.x << "y: " << point.y << std::endl;
                 point.x += x;
                 point.y += y;
                std::cout << "  [New pos] x: " << point.x << "; y: " << point.y << std::endl;
                CGWarpMouseCursorPosition(point);
                std::cout << "  [Shift] X : " << x << "; Y : " << y << "\n";
            }
        }
    }
}
