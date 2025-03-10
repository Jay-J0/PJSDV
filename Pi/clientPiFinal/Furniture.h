
#ifndef FURNITURE_H
#define FURNITURE_H

#include <string>
#include <arpa/inet.h>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include "Message.h"

//mother class of all furnitures (bed, wall, chair, window, etc)
class Furniture {
protected: //protected so we can modify them in the child classes if needed
    int clientSocket;
    const char* ipAddresses;
    Message message; // Composition
    uint16_t conversionValue;

public:
    Furniture(const char* ipAddresses);
    virtual ~Furniture();//virtual destructor (best practice)

    void setSocket(int socket);
    void receiveMessage();
    void sendMessage(uint8_t sendComponentRW, uint8_t sendComponentID, uint8_t sendComponentData1, uint8_t sendComponentData2, uint8_t sendComponentData3);

    uint8_t returnReceiveComponentRW();
    uint8_t returnReceiveComponentID();
    uint8_t returnReceiveComponentData1();
    uint8_t returnReceiveComponentData2();
    uint8_t returnReceiveComponentData3();

    void convertMSGtoUint16();
    uint16_t returnConversionValue();
};

#endif