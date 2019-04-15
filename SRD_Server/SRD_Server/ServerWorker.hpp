#ifndef ServerWorker_hpp
#define ServerWorker_hpp

#include <vector>
#include <memory>

#include "RegularHeaders.h"
#include "NetworkHeaders.h"

class ScreenGrabber;
class InputManager;

// Consider using enum class instead
enum MessageType { MoveMouse = 0, TextMessage };

class ServerWorker : public Poco::Runnable
{
public :
    ServerWorker(SocketAddress&, shared_ptr<InputManager>);
    ~ServerWorker();
    
    void setGrabber(shared_ptr<ScreenGrabber>);
    
    void run() override;
    void stop();
    
    void DestroyClient();
    StreamSocket& getClient() const { return *client;}
    
    void sendFrame(UInt8* frame, int width, int height);
    
private :
    
    const u_long  max_messageBuffer_size,
                  header_size,
                  max_data_len,
                  max_screenFrameMessage_size;
    
    bool finish, hasBytesToServe;
    
    ServerSocket serverSocket;
    shared_ptr<InputManager> inManager;
    shared_ptr<ScreenGrabber> grabber;
    
    StreamSocket* client;
    Timespan timeout;
    
    std::vector<UInt8> messageBuffer;
    
    UInt8* screenFrameMessage;
    
    // Main functions
    void AcceptClient();
    void ReceiveData();
    
    // Utils
    bool CheckAndResizeBuffer(const u_long& received_total, const u_long& offset);
    bool getPackageData(u_long& package_size, int& messageID, const u_long& offset);
    void processPayload(const u_long& msgID, const u_long& offset, const u_long& header_size, const u_long& current_packet_size);
    
};
#endif /* ServerWorker_hpp */
