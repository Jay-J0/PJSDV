#include "Furniture.h"

Furniture::Furniture(const char* ipAddresses) : ipAddresses(ipAddresses) {}

Furniture::~Furniture() {}

void Furniture::setSocket(int socket) {
    this->clientSocket = socket;
}

void Furniture::receiveMessage() {
    message.receivedMessage(clientSocket);
    message.printReceivedMessage();
}

void Furniture::sendMessage(uint8_t RW, uint8_t ID, uint8_t Data1, uint8_t Data2, uint8_t Data3) {
    message.sendMessage(clientSocket, RW, ID, Data1, Data2, Data3);
    message.printSendMessage();
}

uint8_t Furniture::returnReceiveComponentRW() {
    return message.returnReceivedStruct().receivedComponentRW;
}

uint8_t Furniture::returnReceiveComponentID() {
    return message.returnReceivedStruct().receivedComponentID;
}

uint8_t Furniture::returnReceiveComponentData1() {
    return message.returnReceivedStruct().receivedComponentData1;
}

uint8_t Furniture::returnReceiveComponentData2() {
    return message.returnReceivedStruct().receivedComponentData2;
}

uint8_t Furniture::returnReceiveComponentData3() {
    return message.returnReceivedStruct().receivedComponentData3;
}

void Furniture::convertMSGtoUint16(){
  conversionValue = returnReceiveComponentData2();
  conversionValue  = (conversionValue  << 8);
  conversionValue |= returnReceiveComponentData3();
  }

uint16_t Furniture::returnConversionValue(){
  return conversionValue;
}