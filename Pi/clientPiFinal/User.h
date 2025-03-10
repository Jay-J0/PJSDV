#ifndef USER_H
#define USER_H  

#include "Interface.h"

class User : public Interface {
public:
    User(const char* ipAddresses);
    
};

#endif