#ifndef PackageReceiver_hpp
#define PackageReceiver_hpp

#include "RegularHeaders.h"
#include "NetworkHeaders.h"

#include <vector>

class PackageReceiver{
    
public :
    PackageReceiver(StreamSocket* _client,
                    const u_long& _max_messageBuffer_size,
                    const u_long& _header_size,
                    const u_long& _max_data_len);
    
    int readData();
    int processPackage();
    std::pair<uint8_t*, u_long> getLastPayload();
    
    bool getHasBytesToServe() const { return hasBytesToServe; }
    
private :
    const u_long max_messageBuffer_size,
                 header_size,
                 max_data_len;
    
    u_long current_packet_size, received_total, offset;
    u_long lastLenght;
    
    uint8_t* lastStartPos;
    //uint8_t messageID;
    uint messageID;
    
    int received_once;
    bool hasBytesToServe;
    
    StreamSocket* const client; // Can check for nullptr
    std::vector<uint8_t> messageBuffer;
    
    // Utils
    bool CheckAndResizeBuffer();
    void saveLastPackageInfo();
    bool getPackageData();
  
};

#endif /* PackageReceiver_hpp */
