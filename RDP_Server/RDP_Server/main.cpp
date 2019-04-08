#include "RegularHeaders.h"
#include "NetworkHeaders.h"

#include "ClientHandler.hpp"
#include "ClientManager.hpp"

int main()
{
   do {

        // Client manager accepts only one client by default, but you can pass max number of the clients to the constructor
        SocketAddress serverAddress("localhost", 9999);
        ClientManager manager(serverAddress, 100);
        
        StreamSocket client, client2, client3;
        
        client.connect(serverAddress);
        client2.connect(serverAddress);
        client3.connect(serverAddress);
        
        for(int i = 0; i < 2; ++i){
            sleep(1); // Without sleep server receives 8 bytes, which is so strange... Need to be fixed
            
            client.sendBytes("First!", 7);
            
            // They won't send anything if server accepts only one client. Otherwise, everyone will be served
            try {
                client2.sendBytes("Second!", 8);
                client3.sendBytes("Third!", 7);
            }
            catch (...) { }
        }
        cout << "[Main program] Cycle is done\n";
       
        sleep(1);
    } while (true);
    
    return 0;
}



