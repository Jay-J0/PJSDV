// Interface.h
#ifndef INTERFACE_H
#define INTERFACE_H

#include <string>
#include <arpa/inet.h>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include "Message.h"

//mother class of all Interfaces (User, Guard)
class Interface {
protected: //protected so we can modify them in the child classes if needed
    int clientSocket;
    const char* ipAddresses;
    Message message; // Composition
    std::string readData;
    bool alert;
    bool emergency;

public:
    Interface(const char* ipAddresses);
    virtual ~Interface();//virtual destructor (best practice)

    void setSocket(int socket);
    void receiveMessage();
    void sendMessage(uint8_t sendComponentRW, uint8_t sendComponentID, uint8_t sendComponentData1, uint8_t sendComponentData2, uint8_t sendComponentData3);
    uint8_t returnReceiveComponentRW();
    uint8_t returnReceiveComponentID();
    uint8_t returnReceiveComponentData1();
    uint8_t returnReceiveComponentData2();
    uint8_t returnReceiveComponentData3();
    std::string returnReadData();
    void setReadData(std::string setString);
    void setAlert(bool state);
    bool giveAlert();
    void setEmergency(bool state);
    bool giveEmergency();
};

#endif