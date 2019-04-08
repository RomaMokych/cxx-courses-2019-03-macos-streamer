#ifndef ClientHandler_hpp
#define ClientHandler_hpp

#include "RegularHeaders.h"
#include "NetworkHeaders.h"
#include "Poco/NotificationCenter.h"

// Server-side handler
class ClientHandler {
    
public:
    ClientHandler(StreamSocket& ClientSocket, SocketReactor& Reactor, std::atomic<size_t>* clientsCount = nullptr);
    ~ClientHandler();
    
    // Stop receiving anything
    void Destruct();
    
    void onReadable(ReadableNotification* pNf);
    void onShutdown(ShutdownNotification* pNf);
    
    void Destroy();
    
private:
    StreamSocket    socket;
    SocketReactor&  reactor;
    
    std::atomic<size_t>* clients_count;
    bool onReadableDone;
    atomic<bool> destructing;
};

#endif /* ClientHandler_hpp */
