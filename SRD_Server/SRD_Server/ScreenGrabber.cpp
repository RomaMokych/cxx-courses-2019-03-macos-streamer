//
//  ScreenGrabber.cpp
//  SRD_Server
//
//  Created by Nikita on 4/9/19.
//  Copyright © 2019 Nikita. All rights reserved.
//

#include "ScreenGrabber.hpp"
#include "SRD_Server.hpp"

ScreenGrabber::ScreenGrabber(SRD_Server& Server) : server(Server), working(false)
{
}

bool ScreenGrabber::start()
{
    if(working)
        return false;
    
    
    return true;
}

bool ScreenGrabber::stop()
{
    if(!working)
        return false;
    
    
    return true;
}
