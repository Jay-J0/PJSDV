#include "Message.h"

Message::Message() {
    
}

void Message::receivedMessage(int client_fd) {
    recv(client_fd, &receivedM.receivedComponentRW, sizeof(uint8_t), 0);
    recv(client_fd, &receivedM.receivedComponentID, sizeof(uint8_t), 0);
    recv(client_fd, &receivedM.receivedComponentData1, sizeof(uint8_t), 0);
    recv(client_fd, &receivedM.receivedComponentData2, sizeof(uint8_t), 0);
    recv(client_fd, &receivedM.receivedComponentData3, sizeof(uint8_t), 0);
}

void Message::sendMessage(int client_fd, uint8_t sendRW, uint8_t sendID, uint8_t sendData1, uint8_t sendData2, uint8_t sendData3) {
    sendM.sendComponentRW = sendRW;
    sendM.sendComponentID = sendID;
    sendM.sendComponentData1 = sendData1;
    sendM.sendComponentData2 = sendData2;
    sendM.sendComponentData3 = sendData3;


  send(client_fd, reinterpret_cast<const char*>(&sendM), sizeof(MessageComponentsSend), 0);
}

void Message::printReceivedMessage() {
    std::cout << "Received Data: " << std::hex << static_cast<int>(receivedM.receivedComponentRW)
    << " " << static_cast<int>(receivedM.receivedComponentID)
    << " " << static_cast<int>(receivedM.receivedComponentData1)
    << " " << static_cast<int>(receivedM.receivedComponentData2)
    << " " << static_cast<int>(receivedM.receivedComponentData3) << std::dec << "\n";
}

void Message::printSendMessage() {
    std::cout << "Send Data: " << std::hex << static_cast<int>(sendM.sendComponentRW)
    << " " << static_cast<int>(sendM.sendComponentID)
    << " " << static_cast<int>(sendM.sendComponentData1)
    << " " << static_cast<int>(sendM.sendComponentData2)
    << " " << static_cast<int>(sendM.sendComponentData3) << std::dec << "\n";
}

MessageComponentsReceived Message::returnReceivedStruct() {
    return this->receivedM;
}
