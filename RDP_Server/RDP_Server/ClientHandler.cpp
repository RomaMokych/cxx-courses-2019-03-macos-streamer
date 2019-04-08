#include "ClientHandler.hpp"
#include <atomic>

ClientHandler::ClientHandler(StreamSocket& ClientSocket, SocketReactor& Reactor,  std::atomic<size_t>* clientsCount) :
                                                                                    socket(ClientSocket),
                                                                                    reactor(Reactor),

                                                                                    clients_count(clientsCount),
                                                                                    onReadableDone(true),
                                                                                    destructing(false)
{
    
    cout << "[ClientHandler] The client has been accepted successfully!\n";
    
    reactor.addEventHandler(socket, Observer<ClientHandler, ReadableNotification>(*this, &ClientHandler::onReadable));
    reactor.addEventHandler(socket, Observer<ClientHandler, ShutdownNotification>(*this, &ClientHandler::onShutdown));
}

void ClientHandler::onReadable(ReadableNotification* pNf) {
    
    pNf->release();
    
    if(destructing.load())
        return;
    
    onReadableDone = false;
    
    cout << "[ClientHandler] Reading client's data...\n";
    
    char buffer[8];
    
    int n = socket.receiveBytes(buffer, sizeof(buffer));
    cout << "[ClientHandler] Received " << n << " bytes\n";
    
    if (n > 0) {
        cout << "[ClientHandler][Message] " << buffer << "\n\n";
    }
    else
    {
        onReadableDone = true;
        cout << "[ClientHandler] Client isn't connected anymore. Drop the connection\n";
        Destruct();
        
    }
    
    onReadableDone = true;
}

void ClientHandler::onShutdown(ShutdownNotification* pNf)
{
    pNf->release();
    Destruct();
}

void ClientHandler::Destruct()
{
    if(destructing.exchange(true)) // Attempt to enter more than once will be denied
        return;
    
    reactor.removeEventHandler(socket, Observer<ClientHandler, ReadableNotification>(*this, &ClientHandler::onReadable));
    reactor.removeEventHandler(socket, Observer<ClientHandler, ShutdownNotification>(*this, &ClientHandler::onShutdown));
    
    while (!onReadableDone); // Waiting for onReadable to be onReadableDone
    
    delete this;
}

ClientHandler::~ClientHandler()
{
    --*clients_count;
    cout << "[ClientHandler] Handler has been deleted! Left :" << *clients_count << "\n";
}
