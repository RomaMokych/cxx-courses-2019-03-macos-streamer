//
//  MainProgram.hpp
//  Receiver_test_01
//
//  Created by Stas Bespalov on 3/22/19.
//  Copyright © 2019 Nikita. All rights reserved.
//

#ifndef MainProgram_hpp
#define MainProgram_hpp

#include "ScreenGrabber.h"
#include "Server.hpp"

#include <iostream>
#include <chrono>

using namespace std;

class MainProgram{
    
private :
    MyClasses::Server server;
    ScreenGrabber grabber;
    
public:
    MainProgram();
    ~MainProgram();
    
};
#endif /* MainProgram_hpp */
