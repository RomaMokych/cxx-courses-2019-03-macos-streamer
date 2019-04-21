#ifndef Server_hpp
#define Server_hpp

#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/Socket.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/StreamCopier.h"
#include "Poco/Timespan.h"
#include "Poco/FileStream.h"

#include <chrono>
#include <iostream>
//SiTiGi code 03.04.2019
#include <CoreGraphics/CoreGraphics.h>

using namespace std;

using namespace Poco;
using namespace Poco::Net;

namespace MyClasses{
class Server
{
    private :
    ServerSocket serverSocket;
    StreamSocket clientSocket;
    
    Timespan timespan;
    
public:
    
    Server()
    {
        timespan = Timespan(20'000);
        commandBuffer = new uint8_t[12];
    }
    
    ~Server()
    {
        delete[] commandBuffer;
    }
                            
                            void Start();
    void BeginAccept();
    void BeginReceive();
    void SendAll(UInt8* frame, int sizeInBytes);
        
private:
    uint8_t* commandBuffer;
        
};
}


#endif /* Server_hpp */
