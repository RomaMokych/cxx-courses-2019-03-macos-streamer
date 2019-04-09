//
//  ServerWorker.hpp
//  SRD_Server
//
//  Created by Nikita on 4/9/19.
//  Copyright © 2019 Nikita. All rights reserved.
//

#ifndef ServerWorker_hpp
#define ServerWorker_hpp

#include "RegularHeaders.h"
#include "NetworkHeaders.h"

typedef unsigned int UInt;

class ScreenGrabber;

enum MsgID { MoveMouse = 0 };

class ServerWorker : public Poco::Runnable
{
public :
    ServerWorker(ServerSocket& Server, ScreenGrabber& grabber);
    ServerWorker(ServerWorker&);
    ~ServerWorker();
    
    void run() override;
    void stop();
    
    
private :
    
    const UInt maxBufferSize, header_size;
    int received_once, received_total, need_receive;
    
    UInt current_packet_size;
    int messageID;
    
    bool finish;
    
    ServerSocket& server;
    ScreenGrabber& grabber;
    
    StreamSocket* client;
    Timespan timeout;
    
    UInt8* messageBuffer;
    
    void AcceptClient();
    void ReceiveData();
    
};
#endif /* ServerWorker_hpp */
