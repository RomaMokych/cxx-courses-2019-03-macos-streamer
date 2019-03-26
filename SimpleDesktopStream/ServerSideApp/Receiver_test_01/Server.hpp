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
    
    Server(){ timespan = Timespan(20'000); }
    
    void Start(){
        serverSocket.bind(SocketAddress("localhost", 9999));
        serverSocket.listen();
    }
    void BeginAccept();
    void SendAll(UInt8* frame, int sizeInBytes);
};
}


#endif /* Server_hpp */
