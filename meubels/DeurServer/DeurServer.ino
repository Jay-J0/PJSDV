// #include <ESP8266WiFi.h>
// #include <ESP8266mDNS.h>
#include <Wire.h>
#include <Servo.h>
#include "comuni.hpp"

#define I2C_SDL    D1
#define I2C_SDA    D2
//#define NSE

#ifdef NSE 
const char* ssid = "NSELab";  // your network SSID (name)
const char* pass = "NSELabWiFi"; // your network password
IPAddress ip(145, 52, 127, 170); 
IPAddress dns(255, 255, 255, 0);  // not the right one
IPAddress gateway(145, 52, 127, 1);  
IPAddress subnet(255, 255, 255, 0);
#else
const char* ssid = "beverconect";  // your network SSID (name) (alt)
const char* pass = "zoocampbever"; // your network password    (alt)
IPAddress ip(192, 168, 12, 55);  
IPAddress dns(192, 168, 12, 1);
IPAddress gateway(192, 168, 12, 1);
IPAddress subnet(255, 255, 255, 0);
#endif

int status = WL_IDLE_STATUS;

Servo deurServo;

int defaultHex = 0x00; // wire.h transmissions work with hexadecimal values
bool doorOpen = false;
unsigned long doorOpenTime = 0; // deur timer

void openDoor();
void closeDoor();
void outdoorLED(bool aanUit);
void indoorLED(bool aanUit);

msg_t message;
int updatebuttonstatus();
int readButton();
int OutsideButtonState;
int InsideButtonState;


comuni webmos(ssid, pass, ip, dns, gateway, subnet, 5557);

void setup() {
  Serial.begin(74880);
  deurServo.attach(D5);
  pinMode(D5, OUTPUT);

  Wire.begin();
  Wire.beginTransmission(0x38);
  Wire.write(byte(0x03));
  Wire.write(byte(0x0F));
  Wire.endTransmission();

  webmos.connect();
}


void loop() {
  uint8_t dataswitch[3] = {0x00, 0x00, 0x00};
  
  OutsideButtonState = readOutsideButton();  // Initialize OutsideButtonState
  InsideButtonState = readInsideButton();    // Initialize InsideButtonState



    if (webmos.pollserver()) {
      if (OutsideButtonState != 0) {
        dataswitch[2] = 0x01;
        webmos.send_msg(0x81, dataswitch);
        }

      if (InsideButtonState != 0) {
        dataswitch[2] = 0x01;
        webmos.send_msg(0x82, dataswitch);
        }else if (InsideButtonState == 0 && OutsideButtonState == 0) {
        dataswitch[2] = 0x00;
        webmos.send_msg(0x80, dataswitch);
      }

      while (webmos.pollclient()) {
        message = webmos.recv_msg();
        Serial.println(message.comp, HEX);

        if(message.comp == 129 && message.data[2] == 1){
          Serial.println("First IF");
          outdoorLED(true);
          indoorLED(true);
        }

        if(message.comp == 130 && message.data[2] == 0){
         
          outdoorLED(false);
          indoorLED(false);

          doorOpen = !doorOpen;

          // Perform the corresponding action based on the door state
          if (doorOpen) {
            doorOpenTime = millis();
            openDoor(); // open door
          } 
          else {
            closeDoor(); // close door
           }  
        }

        memset(dataswitch, 0, sizeof(dataswitch));

        if (doorOpen && (millis() - doorOpenTime) > 10000) { // close door after 30 seconds 
          closeDoor();
          doorOpen = false;
          }
    } 
  }
}

void openDoor() {
  // Gebruik PWM om de servo naar 150 graden te draaien
  for (int i = 69; i <= 180; i += 1) {
    deurServo.write(i);
    delay(15);
  }

  deurServo.detach();
  deurServo.attach(D5);
}

void closeDoor() {
  // Gebruik PWM om de servo naar 69 graden te draaien
  for (int i = 180; i >= 69; i -= 1) {
    deurServo.write(i);
    delay(15);
  }

  deurServo.detach();
  deurServo.attach(D5);
}

void outdoorLED(bool aanUit) {


  if (aanUit) {
    Wire.beginTransmission(0x38);
    Wire.write(byte(0x01));
    Wire.write(byte(defaultHex |= 0x10));
    Wire.endTransmission();
  } else {
    Wire.beginTransmission(0x38);
    Wire.write(byte(0x01));
    Wire.write(byte(defaultHex &= ~(0x10)));
    Wire.endTransmission();
  }
}

void indoorLED(bool aanUit) {


  if (aanUit) {
    Wire.beginTransmission(0x38);
    Wire.write(byte(0x01));
    Wire.write(byte(defaultHex |= 0x20));
    Wire.endTransmission();
  } else {
    Wire.beginTransmission(0x38);
    Wire.write(byte(0x01));
    Wire.write(byte(defaultHex &= ~(0x20)));
    Wire.endTransmission();
  }
}

int readOutsideButton() {
  Wire.beginTransmission(0x38);
  Wire.write(byte(0x00));
  Wire.endTransmission();
  Wire.requestFrom(0x38, 1); // request data from 0x38
  unsigned int inputs = Wire.read(); // Write value on address of inputs.


  return (inputs & 0x01);
}

int readInsideButton() {
  Wire.beginTransmission(0x38);
  Wire.write(byte(0x00));
  Wire.endTransmission();
  Wire.requestFrom(0x38, 1); // request data from 0x38
  unsigned int inputs = Wire.read(); // Write value on address of inputs.

  return (inputs & 0x02);
}