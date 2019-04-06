#include "RegularHeaders.h"
#include "NetworkHeaders.h"

#include "ClientHandler.hpp"
#include "ClientManager.hpp"

// This example is full of bugs because of multitheading that I'm handling, but this example clearly shows
// that using SocketAcceptor with SocketReactor allows to connect many clients as well as only one client!
int main()
{
    do {
        SocketAddress serverAddress("localhost", 9999);
        ClientManager manager(serverAddress);
        
        // 'ClientManger' accepts more that one client by default,
        // but just change one line of code in the 'ClientHandler' to allow to only one client to be accepted
        
        StreamSocket client, client2, client3;
        
        client.connect(serverAddress);
        client2.connect(serverAddress);
        client3.connect(serverAddress);
        
        for(int i =0;i<5;++i){
            usleep(1'000'000); // Without sleep server receives 8 bytes, which is so strange... Need to be fixed
            
            // Poco's sendBytes(...) guarantees that we'll send as many bytes as we want to be sent
            int s = client.sendBytes("Yay!", 5);
            assert(s == 5);
            
            // They won't send anything if server accepts only one client.
            // Otherwise, everyone will be served
            try {
                client2.sendBytes("Hello!", 7);
                client3.sendBytes("LOL!", 5);
            }
            catch (...) { }
        }
        
        sleep(2);
        cout << "[Main program] Cycle is done\n\n\n";
        
    } while (true);
    
    return 0;
}



