#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <FastLED.h>  //Versie 3.3.0 is niet scuffed?
#include "comuni.hpp"

int RegistreerBeweging(); //Deze functie moet de beweging binnen de radius kunnen registreren, zodat daarna het licht aan kan.
void LampAan(int brightness);
void LampUit();

#define LED_PIN D5  
#define NUM_LEDS 1
CRGB leds[NUM_LEDS];

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
  IPAddress ip(192, 168, 12, 53);
  IPAddress dns(192, 168, 12, 1);
  IPAddress gateway(192, 168, 12, 1);
  IPAddress subnet(255, 255, 255, 0);
#endif

msg_t receivedMessage; 

comuni webmos(ssid, pass, ip, dns, gateway, subnet, 5557); //Laatste is port.

void setup(){
  Wire.begin();

  Serial.begin(74880);

  Wire.beginTransmission(0x36);
  Wire.write(byte(0xA2));
  Wire.write(byte(0x03));
  Wire.endTransmission();

  Wire.beginTransmission(0x38);
  Wire.write(byte(0x03)); // Initialiseert 0 t/m 3 op input.
  Wire.write(byte(0x0F)); // Initialiseert 4 t/m 7 op output.
  Wire.endTransmission();

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS); // Setup voor de lamp
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 500);
  FastLED.clear();
  FastLED.show();

  webmos.connect();
}

void loop(){  
  uint8_t sendComponentData1 = 0x00;
  uint8_t sendComponentData2 = 0x00; 
  uint8_t sendComponentData3 = 0x00;
  
  if (RegistreerBeweging()){
    receivedMessage.comp = 0x41;
    sendComponentData3 = 0x01;
  }

  if (webmos.pollserver()){ // Heb ik überhaupt een connectie?
    uint8_t data[3] = {sendComponentData1, sendComponentData2, sendComponentData3};  

    while (webmos.pollclient()) {
        webmos.send_msg(0x41, data);
    }
    
    if(receivedMessage.comp == 0x41 && sendComponentData3 == 0x01){
      Serial.println("TABLE LAMP ACTIVATED");
      int brightness = 60;
      LampAan(brightness);  
      
      receivedMessage.comp = 0x40;
      sendComponentData3 = 0x00; 
    }  

    if(receivedMessage.comp == 0x41 && sendComponentData3 == 0x00){
      Serial.println("TABLE LAMP DEACTIVATED");
      LampUit();
      
      receivedMessage.comp = 0x40;
      sendComponentData3 = 0x00; 
    }
  }
}

int RegistreerBeweging(){
  Wire.beginTransmission(0x38); //Communicatie via I2C initiëren.
  Wire.write(byte(0x00)); //Er wordt één enkele byte verzonden naar het I2C apparaat.
  Wire.endTransmission(); //I2C communicatie wordt afgesloten.
  Wire.requestFrom(0x38, 1);  //Een request om 1 byte aan data via I2C te ontvangen en klaarmaken om te ontvangen.
  int resultaat = Wire.read();  //De byte uitlezen en opslaan in een resultaat int. 
  return resultaat & 0x01; // &0x01 verzekert dat de teruggegeven waarde van resultaat een 0 of een 1 is. 
}

void LampAan(int brightness){
  leds[0] = CRGB::Orange; //CRGB:: ondersteunt alle HTML geregistreerde kleuren.
  FastLED.setBrightness(brightness);
  FastLED.show(); 
}

void LampUit(){
  FastLED.setBrightness(0); //Een brightness van 0 staat gelijk aan een lamp die uit staat.
  FastLED.show(); //Zonder de show wordt de verandering van aan naar uit alleen nooit zichtbaar.
}
