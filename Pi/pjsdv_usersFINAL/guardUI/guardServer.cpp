#include <netinet/in.h>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 5557
#define SO_REUSEPORT 15

struct MessageReceived{
  uint8_t receivedComponentRW;
  uint8_t receivedComponentID;
  uint8_t receivedComponentData1;
  uint8_t receivedComponentData2;
  uint8_t receivedComponentData3;
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

                recv(new_socket, reinterpret_cast<char *>(&messageFromPi), sizeof(MessageReceived), 0);

                close(new_socket);
                close(server_fd);

            std::cout << "Received Data:\n";
            std::cout << "ComponentRW: " << static_cast<int>(messageFromPi.receivedComponentRW) << "\n";
            std::cout << "sendComponentID: " << static_cast<int>(messageFromPi.receivedComponentID) << "\n";
            std::cout << "sendComponentData1: " << static_cast<int>(messageFromPi.receivedComponentData1) << "\n";
            std::cout << "sendComponentData2: " << static_cast<int>(messageFromPi.receivedComponentData2) << "\n";
            std::cout << "sendComponentData3: " << static_cast<int>(messageFromPi.receivedComponentData3) << "\n";
            std::cout << "\n";
                
                std::string newNotification;
                if (static_cast<int>(messageFromPi.receivedComponentID) == 0x00 && static_cast<int>(messageFromPi.receivedComponentData3) == 0x00) {
                    break;
                }
                if (static_cast<int>(messageFromPi.receivedComponentID) == 0x02 && static_cast<int>(messageFromPi.receivedComponentData3) == 0x01) {
                    newNotification = "ALERT: Emergency alert from Mary";
                }
                if (static_cast<int>(messageFromPi.receivedComponentID) == 0x02 && static_cast<int>(messageFromPi.receivedComponentData3) == 0x02) {
                    newNotification = "ALERT: Burglar in Mary's house";
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
    messageFromPi.receivedComponentRW = 0x00;
    messageFromPi.receivedComponentID = 0x00;
    messageFromPi.receivedComponentData1 = 0x00;
    messageFromPi.receivedComponentData2 = 0x00;
    messageFromPi.receivedComponentData3 = 0x00;
}
