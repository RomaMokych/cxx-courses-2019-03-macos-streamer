#include <iostream>
#include <chrono>
#include <memory>

#include "InputManager.hpp" // Need to have the top order of include. I should fix it

#include "RegularHeaders.h"
#include "NetworkHeaders.h"

#include "SRD_Server.hpp"
#include "ServerWorker.hpp"

#include "ScreenGrabber.hpp"

void launchConsoleLoop() // Need some logic to communicate with server thread
{
    int code = 0;
    
    while(!(code == 9))
    {
        cout << "Please enter command : ";
        cin >> code;
    }
}

int main(int argc, const char * argv[]) {

    SocketAddress serverAddress("10.100.26.130", 2220);
   // SocketAddress serverAddress("192.168.43.112", 2220);
    
    shared_ptr<InputManager>  input;
    shared_ptr<ServerWorker>  worker;
    shared_ptr<ScreenGrabber> grabber;

    input.reset(new InputManager);
    worker.reset(new ServerWorker(serverAddress, input));
    grabber.reset(new ScreenGrabber(worker));

    worker->setGrabber(grabber);
    
    SRD_Server remoteDesktopServer(worker);
    remoteDesktopServer.start();

    // launchConsoleLoop(); // Buggy when setting breakpoints
 
    return 0;
}
