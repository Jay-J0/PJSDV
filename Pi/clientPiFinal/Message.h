#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <arpa/inet.h>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdint>

struct MessageComponentsSend{
    uint8_t sendComponentRW;
    uint8_t sendComponentID;
    uint8_t sendComponentData1;
    uint8_t sendComponentData2;
    uint8_t sendComponentData3;
};

struct MessageComponentsReceived{
    uint8_t receivedComponentRW;
    uint8_t receivedComponentID;
    uint8_t receivedComponentData1;
    uint8_t receivedComponentData2;
    uint8_t receivedComponentData3;
};

class Message {
private:
    MessageComponentsSend sendM;
    MessageComponentsReceived receivedM;


public:
    Message();  // Constructor
    void receivedMessage(int client_fd);
    void sendMessage(int client_fd, uint8_t sendRW, uint8_t sendID, uint8_t sendData1, uint8_t sendData2, uint8_t sendData3);
    void printReceivedMessage();
    void printSendMessage();
    MessageComponentsReceived returnReceivedStruct();
};

#endif
