#include <iostream>

#include "RegularHeaders.h"
#include "NetworkHeaders.h"

#include <chrono>

#include "SRD_Server.hpp"

int main(int argc, const char * argv[]) {

    SocketAddress serverAddress("localhost", 2220);
    SRD_Server remoteDesktopServer(serverAddress);

    remoteDesktopServer.start();
    
    int code = 0;
    
    while(!(code == 9))
    {
        cout << "Please enter command : ";
        cin >> code;
    }
    
    
    return 0;
}
