#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <Wire.h> 


#define I2C_SDL    D1
#define I2C_SDA    D2

//const char *ssid = "NSELab";  // your network SSID (name)
//const char *pass = "NSELabWiFi"; // your network password
const char *ssid = "beverconect";  // your network SSID (name)
const char *pass = "zoocampbever"; // your network password

int status = WL_IDLE_STATUS;

int defaultHex = 0x00; //wire.h transmissions work with hexadecimal values
bool vibrating = false;


//For sending to the client
uint8_t sendComponentRW = 0x00; //0x00 = Read,  0x01 = Write
uint8_t sendComponentID = 0x30; //for chair everything starts with 0x30
uint8_t sendComponentData1 = 0x00;
uint8_t sendComponentData2 = 0x00;
uint8_t sendComponentData3 = 0x00;

//for receiving from the client
struct ReceivedMessageComponents {
  uint8_t receiveComponentRW;
  uint8_t receiveComponentID;
  uint8_t receiveComponentData1;
  uint8_t receiveComponentData2;
  uint8_t receiveComponentData3;
};

ReceivedMessageComponents receivedMessage;

int readButton();
int readSensor();
void vibrateOnOff();

WiFiServer server(5557);

void setup() {

  Wire.begin();

  Serial.begin(74800);
  Serial.println("Attempting to connect to WPA network...");
  Serial.print("SSID: ");
  Serial.println(ssid);

  

  while (status != WL_CONNECTED) {
    status = WiFi.begin(ssid, pass);
    delay(10000);  // Wait for 1 seconds before retrying
    Serial.println("No connection. Retrying...");
  }

  server.begin();
  Serial.print("Connected to wifi. My address:");
  IPAddress myAddress = WiFi.localIP();
  Serial.println(myAddress);
}

void loop() {
  WiFiClient client = server.available();

  int ButtonState = readButton();
  int drukSensor = readSensor();
  
  if (ButtonState == 1 && drukSensor == 1) {

    vibrating = true;
    sendComponentID = 0x31; //hexID value for butten pressed and sensor detection (aka someone is on the chair and wants it to vibrate)
    sendComponentData3= 0x01; //hex value for turn ON
  }

  if(drukSensor == 0 && vibrating == true){
    sendComponentID = 0x31; //hexID no sensor detection and chair vibrating is ON
    sendComponentData3= 0x00; //hex value for turn OFF
  }

  if (client) {
    receivedMessage.receiveComponentRW = 0x00;
    receivedMessage.receiveComponentID = 0x00;
    receivedMessage.receiveComponentData1 = 0x00;
    receivedMessage.receiveComponentData2 = 0x00;
    receivedMessage.receiveComponentData3 = 0x00;

    Serial.println("Pi connected! sending data...");
    client.write(sendComponentRW);
    client.write(sendComponentID);
    client.write(sendComponentData1);
    client.write(sendComponentData2);
    client.write(sendComponentData3);
    Serial.println("Datasend");

    while (client.connected()) {
      if (client.available() >= sizeof(ReceivedMessageComponents)) {
    // Read the entire struct in one go
    client.read(reinterpret_cast<char*>(&receivedMessage), sizeof(ReceivedMessageComponents));

    //Print the received values
    Serial.println(receivedMessage.receiveComponentRW, HEX);
    Serial.println(receivedMessage.receiveComponentID, HEX);
    Serial.println(receivedMessage.receiveComponentData1, HEX);
    Serial.println(receivedMessage.receiveComponentData2, HEX);
    Serial.println(receivedMessage.receiveComponentData3, HEX);
  }
    }
    
    if(receivedMessage.receiveComponentID == 0x31 && receivedMessage.receiveComponentData3 == 0x01){
      Serial.println("COMFY MODE ACTIVATED");
      vibrateOnOff(true);
      sendComponentID = 0x30;
      sendComponentData3 = 0x00; 
    }

    if(receivedMessage.receiveComponentID == 0x31 && receivedMessage.receiveComponentData3 == 0x00){
      Serial.println("COMFY MODE DEACTIVATED");
      vibrateOnOff(false);
      vibrating = false;
      sendComponentID = 0x30;
      sendComponentData3 = 0x00; 
    }

    if(receivedMessage.receiveComponentID == 0x32 && receivedMessage.receiveComponentData3 == 0x01){
      Serial.println("LED ON");
      setLED(true);

      sendComponentID = 0x30;
      sendComponentData3 = 0x00; 
    }

    if(receivedMessage.receiveComponentID == 0x32 && receivedMessage.receiveComponentData3 == 0x00){
      Serial.println("LED OFF");
      setLED(false);

      sendComponentID = 0x30;
      sendComponentData3 = 0x00; 
    }
  }
}

int readButton(){
  Wire.beginTransmission(0x38);
  Wire.write(byte(0x03));          
  Wire.write(byte(0x0F));         
  Wire.endTransmission();
  
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

int readSensor(){
  Wire.beginTransmission(0x36);
  Wire.write(byte(0xA2));          
  Wire.write(byte(0x03));  
  Wire.endTransmission(); 

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

void vibrateOnOff(bool ONorOFF){
  Wire.beginTransmission(0x38);
  Wire.write(byte(0x03));           
  Wire.write(byte(0x0F));            
  Wire.endTransmission();

  if (ONorOFF == true){
    Wire.beginTransmission(0x38);
    Wire.write(byte(0x01));           
    Wire.write(byte(defaultHex |= 0x20));            
    Wire.endTransmission();
  }
  else {
    Wire.beginTransmission(0x38);
    Wire.write(byte(0x01));           
    Wire.write(byte(defaultHex &= ~(0x20)));            
    Wire.endTransmission(); 
  }
}

void setLED(bool ONorOFF){
  Wire.beginTransmission(0x38);
  Wire.write(byte(0x03));           
  Wire.write(byte(0x0F));            
  Wire.endTransmission();

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