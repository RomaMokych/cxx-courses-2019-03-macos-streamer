#include "ClientHandler.hpp"

// Temporary issue : it's static var, so we can't run more than one server at the same time
bool ClientHandler::hasConnection = false;

ClientHandler::ClientHandler(StreamSocket& ClientSocket, SocketReactor& Reactor) :
                                                            socket(ClientSocket),
                                                            reactor(Reactor),
                                                            done(true), destroyed(false)
{
    if (hasConnection) {
        cout << "[ClientHandler] There is only 1 connection's possible. Server's in development. Client has been kicked!\n";
        
        if (destroyed)    // NOTE: TEMPORARY!
            return;
        
        destroyed = true; // NOTE: TEMPORARY!
        delete this;
        return;
    }
    cout << "[ClientHandler] The client has been accepted successfully!\n";
    
    //------ UNCOMMENT THIS TO BE ABLE TO ACCEPT ONLY ONE CLIENT ------
    //hasConnection = true;
    //-------------------------------------------------------------------
    
    reactor.addEventHandler(socket, Observer<ClientHandler, ReadableNotification>(*this, &ClientHandler::onReadable));
    
    // When SocketReactor finishes. Not implemented well yet, hangs.
    // reactor.addEventHandler(socket, Observer<ClientHandler, ShutdownNotification>(*this, &ClientHandler::onShutdown));
}

void ClientHandler::onReadable(ReadableNotification* pNf) {
    
    pNf->release();
    done = false;
    
    cout << "\n[ClientHandler] Reading client's data...\n";
    
    char buffer[8];
    
    int n = socket.receiveBytes(buffer, sizeof(buffer));
    cout << "[ClientHandler] Received " << n << " bytes\n";
    
    if (n > 0) {
        cout << "[ClientHandler][Message] " << buffer << "\n\n";
    }
    else
    {
        done = true;
        cout << "[ClientHandler] Client isn't connected anymore. Drop the connection\n";
        Destruct();
        
        if (destroyed) // NOTE: TEMPORARY!
            return;
        
        // else
        destroyed = true; // NOTE: TEMPORARY!
        delete this;
        return;
        
    }
    
    done = true;
}

// Possible hang somewhere
void ClientHandler::onShutdown(ShutdownNotification* pNf)
{
    if (destroyed) // NOTE: TEMPORARY!
        return;
    
    destroyed = true;
    Destruct();
    cout << "onShutdown()\n";
    delete this;
}

void ClientHandler::Destruct()
{
    if(destroyed) // NOTE: TEMPORARY!
        return;

    reactor.removeEventHandler( socket, Observer<ClientHandler, ReadableNotification>(*this, &ClientHandler::onReadable));
    
    while (!done); // Waiting for onReadable to be done
    
    hasConnection = false;
    cout << "[ClientHandler] Connection has been closed\n";
}

ClientHandler::~ClientHandler() {
    while (!done); // Waiting for onReadable to be done
    cout << "[ClientHandler] Handler is deleted!\n";
}
