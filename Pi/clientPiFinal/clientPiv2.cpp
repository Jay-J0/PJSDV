//PJSDV GROUP 1, ClientPiFinal version 20-01-2024
#include <arpa/inet.h>
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 5557


//#define NSELab
#ifdef NSELab
    //Ip list NSELAB
    #define IP_DOOR "145.52.127.110"
    #define IP_USER "145.52.127.151"
    #define IP_WALL "145.52.127.138"
    #define IP_CHAIR "145.52.127.181"
    #define IP_WINDOW ""
    #define IP_FRIDGE "145.52.127.159"
    #define IP_LAMP "145.52.127.200"  
    #define IP_BED "145.52.127.143"
    #define IP_GUARD "145.52.127.155"

#else
    //IP list Beverconect
    #define IP_DOOR "192.168.12.55"
    #define IP_USER "192.168.12.149"
    #define IP_WALL "192.168.12.57"
    #define IP_CHAIR "192.168.12.109"
    #define IP_WINDOW "192.168.12.54"
    #define IP_FRIDGE "192.168.12.52"
    #define IP_LAMP "192.168.12.53"  
    #define IP_BED "192.168.12.56"
    #define IP_GUARD "192.168.12.1"
#endif

#include "Door.h"
#include "Wall.h"
#include "Chair.h"
#include "User.h"
#include "Guard.h"
#include "Lamp.h"
#include "Fridge.h"
#include "Bed.h"
#include "Window.h"


int main(int argc, char const *argv[])
{
    const char *ipAddresses[] = {IP_DOOR, IP_USER, IP_WALL, IP_CHAIR, IP_WINDOW, IP_FRIDGE, IP_LAMP, IP_BED, IP_GUARD}; //Put all the IP addresses here
    char buffer[1024] = {0};
    Door frontDoor(IP_DOOR);
    Wall wall(IP_WALL);
    Chair comfyChair(IP_CHAIR);
    User mary(IP_USER);
    Guard securityGuard(IP_GUARD);
    Lamp deskLamp(IP_LAMP);
    Fridge smartFridge(IP_FRIDGE);
    Bed bed(IP_BED);
    Window smartWindow(IP_WINDOW);
    

    while (true)//infinite loop
    {
        for (const char *ip : ipAddresses) //loop through all the IP addresses
        {
            int client_fd;
            struct sockaddr_in serv_addr;

            if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
            {
                std::cerr << "\n Socket creation error \n";
                continue; //skip the rest of the loop and go to the next iteration
            }

            serv_addr.sin_family = AF_INET;
            serv_addr.sin_port = htons(PORT);

            // Convert IPv4 and IPv6 addresses from text to binary form
            if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0)
            {
                std::cerr << "\nInvalid address/ Address not supported \n";
                close(client_fd);
                continue; //skip the rest of the loop and go to the next iteration
            }

            if (connect(client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
            {
                std::cerr << "\nConnection Failed for IP " << ip << "\n";
                close(client_fd);
                usleep(250);
                //sleep(1);
                continue; //skip the rest of the loop and go to the next iteration
            }

            if (strcmp(ip, IP_WALL) == 0)
            {
                std::cout << "Performing specific action for " << IP_WALL << "\n";
                wall.setSocket(client_fd);

                wall.receiveMessage();

                //IF Doorbell is pressed, turn on the doorbell
                if((static_cast<int>(frontDoor.returnReceiveComponentID()) == 0x81 && static_cast<int>(frontDoor.returnReceiveComponentData3()) == 0x01) || 
                (static_cast<int>(wall.returnReceiveComponentID()) == 0x12 && static_cast<int>(wall.returnReceiveComponentData3()) == 0x01)){
                    std::cout << "DOORBELL ON\n";
                    if (static_cast<int>(wall.returnReceiveComponentID()) == 0x12 && static_cast<int>(wall.returnReceiveComponentData3()) == 0x01){//heck if the message came from the button = burglar
                        mary.setAlert(true);
                        securityGuard.setAlert(true);
                    }
                    wall.sendMessage(0x00, 0x11, 0x00, 0x00, 0x01); //send message to turn on the doorbell
                }
                else{
                    wall.sendMessage(0x00, 0x11, 0x00, 0x00, 0x00); //send message to turn off the doorbell
                }
            }

            else if (strcmp(ip, IP_WINDOW) == 0)
            {
                std::cout << "Performing specific action for " << IP_WINDOW << "\n";
                smartWindow.setSocket(client_fd);

                smartWindow.receiveMessage();
                smartWindow.convertMSGtoUint16(); //convert the message to uint16_t

                smartWindow.receiveMessage();


                if(smartWindow.returnLDRValue() < 500){ //compare LDR value. if lower than 500, it's night
                    std::cout << "Closing Window Blinds\n";
                    smartWindow.setIsNight(true);
                    smartWindow.sendMessage(0x00, 0x53, 0x00, 0x00, 0x01); //send message to lower blinds
                }
                else if(smartWindow.returnLDRValue() > 500){
                    std::cout << "Opening Window Blinds\n";
                    smartWindow.setIsNight(false);
                    smartWindow.sendMessage(0x00, 0x53, 0x00, 0x00, 0x00); //send message to open blinds
                }

                smartWindow.convertMSGtoUint16();
                smartWindow.setBrightness();

                smartWindow.sendMessage(0x00, 0x54, 0xFF * smartWindow.returnBrightness(), 0xFF * smartWindow.returnBrightness(), 0xFF * smartWindow.returnBrightness()); //data1 = red, data2 = green, data3 = blue
            }

            else if (strcmp(ip, IP_USER) == 0)
            {
                std::cout << "Performing specific action for " << IP_USER << "\n";
                mary.setSocket(client_fd);

                // Receive data from the server
                mary.receiveMessage();

                //emergency button pressed
                if(mary.returnReadData() == "e"){
                    mary.setEmergency(true);
                }

                //Burglar notification
                if(mary.giveAlert() == true){
                    mary.sendMessage(0x00, 0x01, 0x00, 0x00, 0x01);
                    mary.setAlert(false);
                }
                //fridge door open notification
                if(smartFridge.returnDoorAlert() == true){
                    mary.sendMessage(0x00, 0x01, 0x00, 0x00, 0x02);
                    smartFridge.setDoorAlert(false);
                }
            }

            else if (strcmp(ip, IP_GUARD) == 0)
            {
                std::cout << "Performing specific action for " << IP_GUARD << "\n";
                securityGuard.setSocket(client_fd);

                if(mary.giveEmergency() == true){// emergency button pressed by Mary
                    securityGuard.sendMessage(0x00, 0x02, 0x00, 0x00, 0x01);
                    mary.setEmergency(false);
                }

                if(securityGuard.giveAlert() == true){//burglar notification
                    securityGuard.sendMessage(0x00, 0x02, 0x00, 0x00, 0x02);
                    securityGuard.setAlert(false);
                }
            }

            else if (strcmp(ip, IP_DOOR) == 0)
            {
                std::cout << "Performing specific action for " << IP_DOOR << "\n";
                frontDoor.setSocket(client_fd);

                // Parse and store the received data
                frontDoor.receiveMessage();

             if(static_cast<int>(frontDoor.returnReceiveComponentID()) == 0x80 && static_cast<int>(frontDoor.returnReceiveComponentData3()) == 0x00){
                    frontDoor.sendMessage(0x00, 0x80, 0x00, 0x00, 0x00); 
                }

                if(static_cast<int>(frontDoor.returnReceiveComponentID()) == 0x81 && static_cast<int>(frontDoor.returnReceiveComponentData3()) == 0x01){
                    //Doorbell is pressed
                    std::cout << "LED ON\n";
                    frontDoor.sendMessage(0x00, 0x81, 0x00, 0x00, 0x01); //send message to turn ON outside led button
                }
                if((static_cast<int>(frontDoor.returnReceiveComponentID()) == 0x82 && static_cast<int>(frontDoor.returnReceiveComponentData3()) == 0x01 || mary.returnReadData() == "1") ){
                    //Inside door button is pressed
                    std::cout << "LED OFF and open Door\n";
                    frontDoor.sendMessage(0x00, 0x82, 0x00, 0x00, 0x00);//send message to turn OFF outside led button and open/close front door
                    mary.setReadData("0");
                }
            }

            else if (strcmp(ip, IP_BED) == 0)
            {
                std::cout << "Performing specific action for " << IP_BED << "\n";
                bed.setSocket(client_fd);

                // Parse and store the received data
                bed.receiveMessage();

                if(static_cast<int>(bed.returnReceiveComponentID()) == 0x72 && static_cast<int>(bed.returnReceiveComponentData3()) == 0x01){
                    std::cout << "LED ON\n";
                    bed.sendMessage(0x00, 0x72, 0x00, 0x00, 0x01); //send message to turn ON led button
                }
                if(static_cast<int>(bed.returnReceiveComponentID()) == 0x72 && static_cast<int>(bed.returnReceiveComponentData3()) == 0x00){
                    std::cout << "LED OFF \n";
                    bed.sendMessage(0x00, 0x72, 0x00, 0x00, 0x00);//send message to turn OFF led button
                }
            }

            else if (strcmp(ip, IP_FRIDGE) == 0)
            {
                std::cout << "Performing specific action for " << IP_FRIDGE << "\n";
                smartFridge.setSocket(client_fd);

                smartFridge.receiveMessage();
                //update database with recieved data
                if(static_cast<int>(smartFridge.returnReceiveComponentID()) == 0x67 && static_cast<int>(smartFridge.returnReceiveComponentData3()) == 0x01){
                    smartFridge.setDoorAlert(true);
                }

                smartFridge.receiveMessage(); //receive temperature from fridge
                //update database with recieved data
                smartFridge.receiveMessage();
                //update database with recieved data
            }

            else if (strcmp(ip, IP_LAMP) == 0)
            {
                std::cout << "Performing specific action for " << IP_LAMP << "\n";
                deskLamp.setSocket(client_fd);

                // Parse and store the received data
                deskLamp.receiveMessage();

                if(static_cast<int>(deskLamp.returnReceiveComponentID()) == 0x41 && static_cast<int>(deskLamp.returnReceiveComponentData3()) == 0x01){
                    std::cout << "LED ON\n";
                    deskLamp.lampStateON();
                    deskLamp.timerStart();
                    deskLamp.sendMessage(0x00, 0x41, 0x00, 0x00, 0x01); //send message to turn ON led 
                }

                if(deskLamp.returnState() == true){
                    deskLamp.timerStop();
                }


                if(mary.returnReadData() == "2" || deskLamp.giveTime() > 10){
                    std::cout << "LED OFF\n";
                    deskLamp.sendMessage(0x00, 0x41, 0x00, 0x00, 0x00);//send message to turn OFF led 
                    deskLamp.lampStateOFF();
                    deskLamp.timerReset();
                    mary.setReadData("0");
                }
            }

            else if (strcmp(ip, IP_CHAIR) == 0)
            {
                std::cout << "Performing specific action for " << IP_CHAIR << "\n";
                comfyChair.setSocket(client_fd);

                // Parse and store the received data
                comfyChair.receiveMessage();

                if(static_cast<int>(comfyChair.returnReceiveComponentID()) == 0x31 && static_cast<int>(comfyChair.returnReceiveComponentData3()) == 0x01){
                    //Turn on vibrating
                    std::cout << "VIBRATE ON\n";
                    comfyChair.sendMessage(0x00, 0x31, 0x00, 0x00, 0x01); //send message to turn on vibrating
                }

                if(static_cast<int>(comfyChair.returnReceiveComponentID()) == 0x31 && static_cast<int>(comfyChair.returnReceiveComponentData3()) == 0x00){
                    //Turn off vibrating
                    std::cout << "VIBRATE OFF\n";
                    comfyChair.sendMessage(0x00, 0x31, 0x00, 0x00, 0x00); //send message to turn off vibrating
                }

                if(mary.returnReadData() == "3"){
                    //Turn on LED
                    std::cout << "LED ON\n";
                    comfyChair.sendMessage(0x00, 0x32, 0x00, 0x00, 0x01); //send message to turn ON LED
                    mary.setReadData("0");
                }

                if(mary.returnReadData() == "4"){
                    //Turn off LED
                    std::cout << "LED OFF\n";
                    comfyChair.sendMessage(0x00, 0x32, 0x00, 0x00, 0x00); //send message to turn OFF led 
                    mary.setReadData("0");
                }
            }

            close(client_fd);// closing the connected socket

            usleep(250);
        }
    }

    return 0;
}

/* 
REGELS EN AFSPRAKEN:

Standaard uint8_t 

5 BYTES HEX 
[0x00][0x00][0x00][0x00][0x00] 
[rw][sensor/actuator][data1][data2][data3] 
Data word aangepast bij het least significant bit, dus begin bij het meest rechter bitje van data3 (data1, data2, data3)

COMPONENT IDs
0x(meubel)(actuator/sensor) 
0x0... terminal 
0x1... zuil  
0x2... muur 
0x3... stoel 
0x4... schemerlamp
0x5... raam 
0x6... koelkast
0x7... bed 
0x8... deur 

RAAM COMPONENT IDs
0x51 LDR
0x52 Pod meter
0x53 LED-strip

DEUR COMPONENT IDs
0x81 knop buiten
0x82 knop binnen

USER COMPONENT IDs
0x01 Mary
0x02 Guard

ZUIL COMPONENT IDs
0x11 Zoemer

Voor versturen
uint8_t sendComponentRW = 0x00; 
uint8_t sendComponentID = 0x00; 
uint8_t sendComponentData1 = 0x00; 
uint8_t sendComponentData2 = 0x00; 
uint8_t sendComponentData3 = 0x00; 
 
Voor ontvangen
uint8_t receiveComponentRW = 0x00; 
uint8_t receiveComponentID = 0x00;  
uint8_t receiveComponentData1 = 0x00; 
uint8_t receiveComponentData2 = 0x00; 
uint8_t receiveComponentData3 = 0x00; 
*/
