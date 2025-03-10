/*
* Comunication class "comuni.hpp" for furtiture module webmos from the HHS
* 
* Made by PJSW by group 1, Titanic inc.
* 
* Author: Rick ten Wolde, 21028702
* Version: 20240117-2
*/

#include "Print.h"
#include "comuni.hpp"

comuni::comuni(const char* SSID, const char* PASSWD, IPAddress IP, IPAddress DNS, IPAddress GATEWAY, IPAddress SUBNET, uint32_t port): 
ssid(SSID),
passwd(PASSWD),
ip(IP),
dns(DNS),
gateway(GATEWAY),
subnet(SUBNET),    
server(port),
status(WL_IDLE_STATUS){
}


comuni::comuni(const char* SSID, const char* PASSWD, IPAddress IP, IPAddress DNS, IPAddress GATEWAY, uint32_t port): 
ssid(SSID), 
passwd(PASSWD),
ip(IP),
gateway(GATEWAY),
subnet(225, 255, 255, 0), 
server(port),
status(WL_IDLE_STATUS){
}

comuni::~comuni(){  
}
void comuni::send_msg(uint8_t comp, uint8_t data[3]){

  client.write(0x00);
  client.write(comp);
  client.write(data[0]);
  client.write(data[1]);
  client.write(data[2]);

  //Serial.print(comp, HEX);Serial.print(" ");
  //Serial.print(data[0], HEX);Serial.print(" ");
  // Serial.print(data[1], HEX);Serial.print(" ");
  // Serial.print(data[2], HEX);Serial.print(" ");

}
msg_t comuni::recv_msg(){
  //read in a 5 bytes from the TCP window
  client.setTimeout(200);
  client.readBytes((uint8_t*) &in_msg, sizeof(msg_t));

  //Serial.print(in_msg.rw, HEX);Serial.print(" ");
  //Serial.print(in_msg.comp, HEX);Serial.print(" ");
  //Serial.print(in_msg.data[0], HEX);Serial.print(" ");
  //Serial.print(in_msg.data[1], HEX);Serial.print(" ");
  //Serial.print(in_msg.data[2], HEX);Serial.print(" ");
  //Serial.println("");

  return in_msg; 
}

bool comuni::pollserver(){
  client = server.available();
  return client ;
}

bool comuni::pollclient(){
  return client.connected();
}

void comuni::connect(){

    Serial.println("connecting to WPA network...");
    Serial.print("SSID: ");
    Serial.println(ssid);
    
    Serial.print("static IP: ");
    Serial.println(ip);
    Serial.println(gateway);
    Serial.println(subnet);

    WiFi.config(ip, dns, gateway);
    status = WiFi.begin(ssid, passwd);

    while (WiFi.status() != WL_CONNECTED){   
    delay(1000);  // Wait for 1 second(s) before printing to Serial monitor
    Serial.print(status);
    Serial.println("  No connection. Retrying... ");
    }

    server.begin(); 
    Serial.print("Connected to wifi. My address:");
    IPAddress myAddress = WiFi.localIP();
    Serial.println(myAddress);
    
  }