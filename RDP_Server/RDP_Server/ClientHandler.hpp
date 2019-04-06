#include "RegularHeaders.h"
#include "NetworkHeaders.h"


#ifndef ClientHandler_hpp
#define ClientHandler_hpp

// Server-side handler
class ClientHandler {
    
public:
    ClientHandler(StreamSocket& ClientSocket, SocketReactor& Reactor);
    ~ClientHandler();
    
    // Stop receiving anything
    void Destruct();
    
    void onReadable(ReadableNotification* pNf);
    void onShutdown(ShutdownNotification* pNf);
    
    void Destroy();
    
private:
    StreamSocket    socket;
    SocketReactor&  reactor;
    
    bool done, destroyed;
    static bool hasConnection;
};

#endif /* ClientHandler_hpp */
