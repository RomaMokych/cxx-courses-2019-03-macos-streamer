#include "MainProgram.hpp"

MainProgram::MainProgram() : server(),
                             grabber(server) {  // Screengrabber can send frames through the server
    server.Start();
    server.BeginAccept(); // NO MULTITHREADING! Busy waits for any connection (TCP)
    
    grabber.StartStream();
    
    // If we don't wait, MainProgram dies
    sleep(60*10); // So, wait 10 mins. Just for example.
    
    }

MainProgram::~MainProgram(){
    grabber.EndStream();
    }
