#ifndef ServerWorker_hpp
#define ServerWorker_hpp

#include <vector>
#include <memory>

#include "RegularHeaders.h"
#include "NetworkHeaders.h"

#include "PackageReceiver.hpp"

class ScreenGrabber;
class InputManager;

// Move to some another place
// Consider using enum class instead
enum MessageType {
    ClickLeftMouseButton = 1,
    DoubleClickLeftMouseButton,
    ClickRightMouseButton,
    HoldLeftMouseButton,
    MoveHeldMouse,
    FreeLeftMouseButton,
    MoveMouseByVector,
    PressKeyChar,
    PressKeyTab,
    PressSpecialKeyTab,
    Scroll = 11
};

class ServerWorker : public Poco::Runnable
{
public :
    ServerWorker(SocketAddress&, shared_ptr<InputManager>);
    ~ServerWorker();
    
    void setGrabber(shared_ptr<ScreenGrabber>);
    
    void run() override;
    void stop();
    
    void sendData(UInt8*, const u_long&);
    
    void DestroyClient();
    
private :
    
    const u_long  max_messageBuffer_size,
                  header_size,
                  max_data_len,
                  max_screenFrameMessage_size;
    
    bool finish;
    
    //shared_ptr<ServerSocket> serverSocket;
    ServerSocket serverSocket;
    shared_ptr<InputManager> inputManager;
    shared_ptr<ScreenGrabber> screenGrabber;
    
    //shared_ptr<StreamSocket> client;
    StreamSocket* client;
    
    PackageReceiver* packageReceiver;
    
    Timespan timeout;
    
    std::vector<UInt8> messageBuffer;
    
    UInt8* screenFrameMessage;
    
    // Main functions
    bool AcceptClient();
    void ReceiveData();
    
    // Utils
    bool CheckAndResizeBuffer(const u_long& received_total, const u_long& offset);
    bool getPackageData(u_long& package_size, int& messageID, const u_long& offset);
    
    void processPayload(const u_long& msgID,
                        const uint8_t* startOfPayload,
                        const u_long& lengthOfPayload);
    
};
#endif /* ServerWorker_hpp */
