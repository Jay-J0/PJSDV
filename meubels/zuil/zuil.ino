/*
* "zuil" code for Wemos modules
* 
* Made by PJSW by group 1, Titanic inc.
* 
* Author(s): J.J. Niesen, Metehan Akin
* cleanUP + class implementation: Rick ten Wolde
* Version: 20240118-1
*/

#include <Wire.h>
#include <ESP8266WiFi.h>

#include "comuni.hpp"

#define SDA_PIN D1
#define SCL_PIN D2

//#define NSE
#ifdef NSE 
  const char *ssid = "NSELab";  // your network SSID (name)
  const char *pass = "NSELabWiFi"; // your network password
  IPAddress ip(145, 52, 127, 159); 
  IPAddress dns(255, 255, 255, 0);//not the right one
  IPAddress gateway(145, 52, 127, 1);  
  IPAddress subnet(255, 255, 255, 0);
#else
  const char *ssid = "beverconect";  // your network SSID (name) (alt)
  const char *pass = "zoocampbever"; // your network password    (alt)
  IPAddress ip(192, 168, 12, 57);  
  IPAddress dns(192, 168, 12, 1);
  IPAddress gateway(192, 168, 12, 1);
  IPAddress subnet(255, 255, 255, 0);
#endif

msg_t receivedMessage;

int defaultHex = 0x00; 

void turnOnBuzzer();
void turnOffBuzzer();
void BuzzerOnOff(int i);
bool readButton();
unsigned int readAnalogSensor(int ANALOG_CH);
void initi2c();

unsigned int outputState = 0;
//int buttonState;

comuni webmos(ssid, pass, ip, dns, gateway, subnet, 5557);

void setup(){
  pinMode(A0, INPUT);
  pinMode(D0, INPUT);
  Wire.begin();
  initi2c();
  BuzzerOnOff(0);

  Serial.begin(75880);//was 115200?
  webmos.connect();
}
uint8_t data[3] = {0x00};

void loop() {
 
  if(readButton()){
      //sendComponentID = 0x12;
    data[2] = 0x01;    
    Serial.println("press button");
  }else{
    data[2] = 0x00; 
  }

  if (webmos.pollserver()) {
    while (webmos.pollclient()){
      // Read the entire struct in one go       
      webmos.send_msg(0x12,data);
      receivedMessage = webmos.recv_msg();

      if(receivedMessage.comp == 0x11 && receivedMessage.data[2] == 0x01){ //Turn ON doorbell
      Serial.println("Doorbell ON");
      BuzzerOnOff(1);    
      }

      if(receivedMessage.comp == 0x11 && receivedMessage.data[2] == 0x00){ //Turn OFF dorbell
      Serial.println("Doorbell OFF");
      BuzzerOnOff(0);
      }      
    }    
  }
}

// Functie om analoge input te lezen van MAX11647
unsigned int readAnalogSensor(int ANALOG_CH) {
  unsigned int sensorValue0;
  unsigned int sensorValue1;
  Wire.requestFrom(0x36, 4);

  // Lees MAX11647
  if (ANALOG_CH == 0) {
    sensorValue0 = Wire.read() & 0x03;
    sensorValue0 = sensorValue0 << 8;
    sensorValue0 = sensorValue0 | Wire.read();
    return sensorValue0;
  }

  if (ANALOG_CH == 1) {
    Wire.read();
    Wire.read();
    sensorValue1 = Wire.read() & 0x03;
    sensorValue1 = sensorValue1 << 8;
    sensorValue1 = sensorValue1 | Wire.read();
    return sensorValue1;
  }
  return 0;
}

void BuzzerOnOff(int i) {
  if (i == 1) { //turn on
    Wire.beginTransmission(0x38);
    Wire.write(byte(0x01));
    Wire.write(byte(defaultHex |= 0x10));
    Wire.endTransmission();
  }
  else { //turn off
    Wire.beginTransmission(0x38);
    Wire.write(byte(0x01));
    Wire.write(byte(defaultHex &= !(0x10)));
    Wire.endTransmission();
  }
}

bool readButton(){ 
  Wire.beginTransmission(0x38); 
  Wire.write(byte(0x00));      
  Wire.endTransmission();
  Wire.requestFrom(0x38, 1);  //request data from 0x38
  unsigned int inputs = Wire.read();//Write value on address of inputs.

  if ((inputs&0x01) == 1){
    return 1;
  }
  else {
    return 0;
  }
}

void initi2c(){
  Wire.beginTransmission(0x38);
  Wire.write(byte(0x03)); // Initialiseert 0 t/m 3 op input.
  Wire.write(byte(0x0F)); // Initialiseert 4 t/m 7 op output.
  Wire.endTransmission();
}
