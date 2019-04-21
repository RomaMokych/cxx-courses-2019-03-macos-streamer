#include "DataPackage.hpp"

// Fills package with given data. Destroys previous data.
bool DataPackage::fill(const u_long& _length, const uint8_t& _type, const uint8_t* _payload)
{
    this->length = _length;
    this->type = _type;
    
    
    if(ready_package) // Clear package if it already contains any data
        delete[] ready_package;
    
    try
    {
        // Allocate space to store whole package in one buffer
        ready_package = new uint8_t[sizeof(length) + sizeof(type) + length];
    }
    catch(std::bad_alloc&)
    {
        reset();
        return false;
    }
    
    u_long offset = 0;
    bool success = true;
    
    success &= write_to_from(ready_package + offset, reinterpret_cast<uint8_t*>(&length), sizeof(length)); // Write length of package
    offset += sizeof(length);
    
    success &= write_to_from(ready_package + offset, &type, sizeof(type)); // Write type of package
    offset += sizeof(type);
    
    success &= write_to_from(ready_package + offset, _payload, length); // Write payload
    
    if(!success) // If at least one operation fails, package becomes empty
    {
        reset();
        return false;
    }
    
    return true;
}

void DataPackage::reset()
{
    this->length = 0;
    this->type = 0;
    ready_package = 0;
}

bool DataPackage::write_to_from(uint8_t* to, const uint8_t* const from, const u_long& count)
{
    void* result = NULL;
    
    result = memcpy(to, from, count); // memcpy returns pointer to destination point if case of success
    
    if(result != to)
        return false;
    
    return true;
}


DataPackage::~DataPackage()
 {
     if(ready_package)
         delete[] ready_package;
 }
