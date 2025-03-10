/*
* Comunication class "comuni.cpp" for furtiture module webmos from the HHS
* 
* Made by PJSW by group 1, Titanic inc.
* 
* Author: Rick ten Wolde, 21028702
* Version: 20240117-2
*/

#ifndef COMUNI_H
#define COMUNI_H


#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <Arduino.h>

typedef struct{
  uint8_t rw = 0x00;
  uint8_t comp = 0x00;
  uint8_t data[3] = {0x00,0x00, 0x00};
} msg_t;



class comuni{
  private:

    const char* ssid;
    const char* passwd;

    IPAddress ip; 
    IPAddress dns; 
    IPAddress gateway; 
    IPAddress subnet; 

    int status;
    WiFiServer server;
    WiFiClient client;
    msg_t in_msg;
    msg_t out_msg;    

  public:
  comuni(const char* SSID, const char* PASSWD, IPAddress IP, IPAddress DNS, IPAddress GATEWAY, IPAddress SUBNET, uint32_t port);//manual subnet
  comuni(const char* SSID, const char* PASSWD, IPAddress IP, IPAddress DNS, IPAddress GATEWAY,                   uint32_t port); //default /24 subnet 
  ~comuni();

  //dont forget to add static ip, dns, gateway in the *.ino
  void connect();
  void send_msg(uint8_t comp, uint8_t data[3]); //manualy fill component ID and array o
  msg_t recv_msg();

  bool pollserver(); //check if client is avalible
  bool pollclient(); //check if client is avalible

};

#endif
