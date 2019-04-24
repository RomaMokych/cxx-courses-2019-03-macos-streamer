#ifndef DataPackage_hpp
#define DataPackage_hpp

#include "RegularHeaders.h"

class DataPackage {
    
public :
    DataPackage() : length(0), type(0) , ready_package(nullptr) { }
    ~DataPackage();
    
    bool fill(const u_long& length, const uint8_t& type, const uint8_t* payload);
    void reset();
    
    // Read only
    inline operator const uint8_t*() const { return ready_package ? ready_package : nullptr; }

private :
    u_long length;
    uint8_t type;
    uint8_t* ready_package;
    
    bool write_to_from(uint8_t* to, const uint8_t* const from, const u_long& count);
   
};
#endif /* DataPackage_hpp */
