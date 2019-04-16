#include <iostream>
#include <chrono>
#include <memory>

#include "InputManager.hpp"

#include "RegularHeaders.h"
#include "NetworkHeaders.h"


#include "ServerWorker.hpp"
#include "ScreenGrabber.hpp"
#include "SRD_Server.hpp"

void launchConsoleLoop()
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

    shared_ptr<InputManager>  input;
    shared_ptr<ServerWorker>  worker;
    shared_ptr<ScreenGrabber> grabber;

    input.reset(new InputManager);
    input->press_LeftMouseButton(true);
    worker.reset(new ServerWorker(serverAddress, input));
    grabber.reset(new ScreenGrabber(worker));

    worker->setGrabber(grabber);

    input->press_LeftMouseButton(false);
    
    SRD_Server remoteDesktopServer(worker);
    remoteDesktopServer.start();

    launchConsoleLoop();
 
    return 0;
}
