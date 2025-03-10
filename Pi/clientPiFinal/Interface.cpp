#include "Interface.h"

Interface::Interface(const char* ipAddresses) : ipAddresses(ipAddresses) {}

Interface::~Interface() {}

void Interface::setSocket(int socket) {
    this->clientSocket = socket;
}

void Interface::receiveMessage() {
    char buffer[1024] = {0};
    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

    if (bytesRead > 0) {
        buffer[bytesRead] = '\0';
        readData = std::string(buffer);
        std::cout << "Data received: " << readData << "\n";
    }
}

void Interface::sendMessage(uint8_t RW, uint8_t ID, uint8_t Data1, uint8_t Data2, uint8_t Data3) {
    message.sendMessage(clientSocket, RW, ID, Data1, Data2, Data3);
    message.printSendMessage();
}

uint8_t Interface::returnReceiveComponentRW() {
    return message.returnReceivedStruct().receivedComponentRW;
}

uint8_t Interface::returnReceiveComponentID() {
    return message.returnReceivedStruct().receivedComponentID;
}

uint8_t Interface::returnReceiveComponentData1() {
    return message.returnReceivedStruct().receivedComponentData1;
}

uint8_t Interface::returnReceiveComponentData2() {
    return message.returnReceivedStruct().receivedComponentData2;
}

uint8_t Interface::returnReceiveComponentData3() {
    return message.returnReceivedStruct().receivedComponentData3;
}

std::string Interface::returnReadData() {
    return readData;
}

void Interface::setReadData(std::string setString) {
    readData = setString;
}

bool Interface::giveAlert(){
    return alert;
}

void Interface::setAlert(bool state){
  alert = state;
}

void Interface::setEmergency(bool state){
    emergency = state;
}

bool Interface::giveEmergency(){
    return emergency;
}