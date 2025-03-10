#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <Wire.h>
#include "comuni.hpp"

#define I2C_SDL    D1
#define I2C_SDA    D2

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
  IPAddress ip(192, 168, 12, 56);  
  IPAddress dns(192, 168, 12, 1);
  IPAddress gateway(192, 168, 12, 1);
  IPAddress subnet(255, 255, 255, 0);
#endif

int defaultHex = 0x00; //wire.h transmissions work with hexadecimal values
bool led = false;

bool readButton();
int readSensor();
void setLED(bool ONorOFF);

msg_t in_msg;

uint8_t compid = 0;
uint8_t msgdata[3] = {0x00};

comuni webmos(ssid, pass, ip, dns, gateway, subnet, 5557);

void setup() {
  Wire.begin();

  Serial.begin(74880);

  initi2c();

  webmos.connect();
}

void loop(){
   
  if (readButton()){
    // Knop is ingedrukt   
    led  = !led;
    if(led){
      compid = 0x72; //hexID value for button pressed
      msgdata[2] = 0x01; //hex value for turn ON
    
    //setLED(true);
    }else{
      // Knop is niet ingedrukt
      compid = 0x72; //hexID value for button released
      msgdata[2] = 0x00; //hex value for turn OFF
      setLED(false);
    }
  } 

  if(webmos.pollserver()) {   
    while (webmos.pollclient()){
      webmos.send_msg(compid,msgdata);
      in_msg = webmos.recv_msg();

      if(in_msg.comp == 0x72 && in_msg.data[2] == 0x01){
       // Serial.println("LED ON");
        setLED(true);

        compid = 0x70; //hexID value for button released
        msgdata[2] = 0x00; //hex value for turn OFF    

      }else if(in_msg.comp == 0x72 && in_msg.data[2] == 0x00){
      //  Serial.println("LED OFF");
        setLED(false);
      }   
    }
  }
}

void initi2c(){
  Wire.beginTransmission(0x36); // Het beginnen van een I2C transmissie naar het apparaat met het meegegeven adres. 
  Wire.write(byte(0xA2));
  Wire.write(byte(0x03));
  Wire.endTransmission(); // Het daadwerkelijke versturen van de bytes.

  Wire.beginTransmission(0x38);
  Wire.write(byte(0x03)); // Initialiseert 0 t/m 3 op input.
  Wire.write(byte(0x0F)); // Initialiseert 4 t/m 7 op output.
  Wire.endTransmission();
}

bool readButton(){
  
  Wire.beginTransmission(0x38); 
  Wire.write(byte(0x00));      
  Wire.endTransmission();
  Wire.requestFrom(0x38, 1);  //request data from 0x38
  unsigned int inputs = Wire.read();//Write value on address of inputs.

  if(inputs & 0x01 == 0x01){
    return 1;
  }else {
    return 0;
  }
}

int readSensor(){
  Wire.requestFrom(0x36, 4);  //request data from 0x36
  unsigned int anin0 = Wire.read()&0x03;
  anin0=anin0<<8;
  anin0 = anin0|Wire.read();
  if (anin0 >= 100){
    return 1;
  }
  else {
    return 0;
  }
}

void setLED(bool ONorOFF){
  
  if (ONorOFF == true){
    Wire.beginTransmission(0x38);
    Wire.write(byte(0x01));           
    Wire.write(byte(defaultHex |= 0x10));            
    Wire.endTransmission();
  }
  else {
    Wire.beginTransmission(0x38);
    Wire.write(byte(0x01));           
    Wire.write(byte(defaultHex &= ~(0x10)));            
    Wire.endTransmission(); 
  }
}
