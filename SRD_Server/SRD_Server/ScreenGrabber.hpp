//
//  ScreenGrabber.hpp
//  SRD_Server
//
//  Created by Nikita on 4/9/19.
//  Copyright © 2019 Nikita. All rights reserved.
//

#ifndef ScreenGrabber_hpp
#define ScreenGrabber_hpp

#include "RegularHeaders.h"
#include "NetworkHeaders.h"

class SRD_Server;

class ScreenGrabber
{
public :
    ScreenGrabber(SRD_Server& server);
    bool start();
    bool stop();
    
private :
    SRD_Server& server;
    
    bool working;
};


#endif /* ScreenGrabber_hpp */
