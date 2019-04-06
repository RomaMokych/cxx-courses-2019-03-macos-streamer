#include "MainProgram.hpp"

MainProgram::MainProgram() : server(),
                             grabber(server) {  // Screengrabber can send frames through the server
    server.Start();
    server.BeginAccept(); // NO MULTITHREADING! Busy waits for any connection (TCP)
    
    grabber.StartStream();
    server.BeginReceive(); // Program will hang
    
    }

MainProgram::~MainProgram(){
    grabber.EndStream();
    }
