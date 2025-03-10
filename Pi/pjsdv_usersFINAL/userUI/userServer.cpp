#include <netinet/in.h>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 5557
#define SO_REUSEPORT 15

struct MessageReceived{
  uint8_t receivedsendComponentRW;
  uint8_t receivedsendComponentID;
  uint8_t receivedsendComponentData1;
  uint8_t receivedsendComponentData2;
  uint8_t receivedsendComponentData3;
};

MessageReceived messageFromPi;

void resetReceivedMessage();

int main() {
    int server_fd = -1, new_socket = -1;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    char buffer[1024] = {0};
    std::string sendData;
    std::string notifications;

    while (true) {
        std::cout << "Notifications:\n";
        std::cout << "------------------------------------------";
        std::cout << notifications << "\n";
        std::cout << "------------------------------------------\n";
        std::cout << "Send the number of the command to perform the action!\n";
        std::cout << "Enter: Reload notifications\n";
        std::cout << "e: Emergency\n";
        std::cout << "z: Clear notifications\n";
        std::cout << "1: Open/close Door\n";
        std::cout << "2: Turn OFF Desklamp\n";
        std::cout << "3: Turn on LED chair\n";
        std::cout << "4: Turn OFF LED chair\n";

        std::cout << "Waiting for user input...\n";
        std::string userInput;
        std::getline(std::cin, userInput); // Wait for user input

        if (userInput == "z") {
            std::cout << "Emptying notifications...\n";
            notifications = "";
            continue;
        }

        if (userInput.empty()) { //if enter is pressed
            continue;
        }

        // Creating socket file descriptor for PORT
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            perror("socket failed"); 
            exit(EXIT_FAILURE);
        }

        // Forcefully attaching socket to the PORT
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(PORT);

        // Forcefully attaching socket to the PORT
        if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
            perror("bind failed");
            exit(EXIT_FAILURE);
        }
        if (listen(server_fd, 3) < 0) {
            perror("listen");
            exit(EXIT_FAILURE);
        }

        while (true) {
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen)) >= 0) {

                send(new_socket, userInput.c_str(), userInput.length(), 0);
                std::cout << "Data sent: " << userInput << "\n";
                std::cout << "\n";

                recv(new_socket, reinterpret_cast<char *>(&messageFromPi), sizeof(MessageReceived), 0);

                close(new_socket);
                close(server_fd);
                
                std::string newNotification;
                if (static_cast<int>(messageFromPi.receivedsendComponentID) == 0x01 && static_cast<int>(messageFromPi.receivedsendComponentData3) == 0x01) {
                    newNotification = "ALERT: Burglar";
                }
                if (static_cast<int>(messageFromPi.receivedsendComponentID) == 0x01 && static_cast<int>(messageFromPi.receivedsendComponentData3) == 0x02) {
                    newNotification = "Fridge door open";
                }
                if (!newNotification.empty()) {
                        notifications += "\n";
                    notifications += newNotification;
                }

                resetReceivedMessage();
                break;
            }
        }
    }
    return 0;
}

void resetReceivedMessage() {
    messageFromPi.receivedsendComponentRW = 0x00;
    messageFromPi.receivedsendComponentID = 0x00;
    messageFromPi.receivedsendComponentData1 = 0x00;
    messageFromPi.receivedsendComponentData2 = 0x00;
    messageFromPi.receivedsendComponentData3 = 0x00;
}
