#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <Wire.h> 

#include "comuni.hpp"

#define I2C_SDL D1
#define I2C_SDA D2

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
  IPAddress ip(192, 168, 12, 52);  
  IPAddress dns(192, 168, 12, 1);
  IPAddress gateway(192, 168, 12, 1);
  IPAddress subnet(255, 255, 255, 0);
#endif

//function prototypes
uint8_t read_switch();
void init_i2c();
void print_temps();
void set_fan_ON(); //should always be used!
void set_fan_OFF(); //dont use when D5 is on HIGH!!!!
float readtemp_inside();
float readtemp_outside();
uint16_t readtemp_inside_raw();
uint16_t readtemp_outside_raw();
void fanspeed(uint8_t sec); //dont use might break peltier

comuni webmos(ssid, pass,ip, dns ,gateway, subnet, 5557); //use default subnet of /24

void setup() {
 Serial.begin(74880); // enable serial output
 init_i2c();

 pinMode(D5, OUTPUT); //Peltier pin on output 
 digitalWrite(D5,HIGH);

 webmos.connect();
 
 set_fan_ON(); 
}
  
void loop() {

  uint8_t datainside[3] = {0x00,0x00, 0x00}; //convert tempeture read to msg data
  uint16_t temp_in = readtemp_inside_raw();
  datainside[1] = (uint8_t)(temp_in >> 8); 
  datainside[2] = (uint8_t)(temp_in); 

  uint8_t dataoutside[3] = {0x00,0x00, 0x00}; //convert tempeture read to msg data
  uint16_t temp_out = readtemp_outside_raw();
  dataoutside[1] = (uint8_t)(temp_out >> 8); 
  dataoutside[2] = (uint8_t)(temp_out); 

  uint8_t dataswitch[3] = {0x00,0x00, 0x00};
  dataswitch[2] = read_switch();

  if(webmos.pollserver()){
    while(webmos.pollclient()){      
      webmos.send_msg(0x67, dataswitch);    
      webmos.send_msg(0x68, datainside);  
      webmos.send_msg(0x69,dataoutside);   
    }
  }
}

uint8_t read_switch(){
  Wire.beginTransmission(0x38); 
  Wire.write(byte(0x00));      
  Wire.endTransmission();
  Wire.requestFrom(0x38, 1);  //request data from 0x38
  unsigned int input = Wire.read();//Write value on address of inputs.

  if(input&0x02 == 0x02){
    return 0;
  }else {
    return 1;
  }
}

void init_i2c(){
  Wire.begin(); //enable i2c
  Wire.beginTransmission(0x38); //init i2c I/O expander
  Wire.write(byte(0x03));         
  Wire.write(byte(0x0F)); //set DDR to 0xoutput|input        
  Wire.endTransmission();

  Wire.beginTransmission(0x36); //init analoge input
  Wire.write(byte(0xA2));          
  Wire.write(byte(0x03));  
  Wire.endTransmission(); 
}

void print_temps(){
  Serial.print("inside temp : ");
  Serial.print(readtemp_inside());   
  Serial.print("c | ");
  Serial.print("outside temp: ");
  Serial.print(readtemp_outside());   
  Serial.println("c ");
}

void set_fan_ON(){
  Wire.beginTransmission(0x38); 
  Wire.write(byte(0x01));            
  Wire.write(byte(0x10));            
  Wire.endTransmission(); 
}

void set_fan_OFF(){
  Wire.beginTransmission(0x38); 
  Wire.write(byte(0x01));            
  Wire.write(byte(0x00));            
  Wire.endTransmission(); 
}

float readtemp_inside(){
  Wire.requestFrom(0x36, 4);  
  Wire.read();// skip first 2 bytes  to acces analogue 1 hi|lo
  Wire.read();
  uint32_t AI1 = Wire.read()& 0x03;  
  AI1=AI1<<8;
  AI1 = AI1|Wire.read();
  return (0.22*AI1) + -90.5; // calc to c 
}

float readtemp_outside(){
  Wire.requestFrom(0x36, 4);  
  uint32_t AI0 = Wire.read()& 0x03;  
  AI0=AI0<<8;
  AI0 = AI0|Wire.read();
  return (0.22*AI0) + -90.5; // calc to c
}

uint16_t readtemp_inside_raw(){
  Wire.requestFrom(0x36, 4);  
  Wire.read();
  Wire.read();
  uint16_t AI1 = Wire.read()& 0x03;  
  AI1=AI1<<8;
  AI1 = AI1|Wire.read();
  return AI1; // calc to c 
}

uint16_t readtemp_outside_raw(){
  Wire.requestFrom(0x36, 4);  
  uint16_t AI0 = Wire.read()& 0x03;  
  AI0=AI0<<8;
  AI0 = AI0|Wire.read();
  return AI0; // calc to c ;
}

void fanspeed(uint8_t sec){
  set_fan_ON();
  delay(sec*100);
  set_fan_OFF();
  delay(sec*100);
}