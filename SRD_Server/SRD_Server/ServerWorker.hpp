#ifndef ServerWorker_hpp
#define ServerWorker_hpp

#include <vector>

#include "RegularHeaders.h"
#include "NetworkHeaders.h"

class ScreenGrabber;

enum MsgID { MoveMouse = 0, TextMessage = 1 };

class ServerWorker : public Poco::Runnable
{
public :
    ServerWorker(ServerSocket& Server, ScreenGrabber& grabber);
    ~ServerWorker();
    
    void run() override;
    void stop();
    void DestroyClient();
    
    StreamSocket* getClient() const { return client;}
    
private :
    
    const u_long  max_messageBuffer_size,
                  header_size,
                  max_data_len;
    
    bool finish, hasBytesToServe;
    
    ServerSocket& server;
    ScreenGrabber& grabber;
    
    StreamSocket* client;
    Timespan timeout;
    
    std::vector<UInt8> messageBuffer;
    
    // Main functions
    void AcceptClient();
    void ReceiveData();
    
    // Utils
    bool CheckAndResizeBuffer(const u_long& received_total, const u_long& offset);
    bool getPackageData(u_long& package_size, int& messageID, const u_long& offset);
    
};
#endif /* ServerWorker_hpp */
