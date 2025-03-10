#include <Wire.h>
#include <FastLED.h>  //Versie 3.3.0 is niet scuffed?
#include "comuni.hpp"

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
  IPAddress ip(192, 168, 12, 54);  
  IPAddress dns(192, 168, 12, 1);
  IPAddress gateway(192, 168, 12, 1);
  IPAddress subnet(255, 255, 255, 0);
#endif

//Defines voor de LED strip.
#define LED_PIN D5 
#define NUM_LEDS 3
CRGB leds[NUM_LEDS];

void initi2c();
void intiLEDS();

void RegelBlindering(); //Moet gaan zorgen voor de conditie voor de switch tussen omhoog en omlaag.
void RegelLichtsterkte(uint8_t R, uint8_t G, uint8_t B); // Moet gaan zorgen voor verschillende sterkten van licht als je aan de dimmer draait. 
void RegelLichtshow(); // Zorgt voor verschillende kleuren licht op de led.

uint16_t LdrUitlezen(); //Geeft de waarde van de lichtsensor terug.
uint16_t DimmerUitlezen(); //Geeft de waarde van de dimmer/potmeter terug. 

void BlinderingOmlaag(); //Moet ervoor gaan zorgen dat het raampje zwart wordt.
void BlinderingOmhoog(); //Moet ervoor gaan zorgen dat het raampje doorzichtig wordt

uint8_t sendingDataLDR[3]= {0};
uint8_t sendingDataPOD[3]= {0};

msg_t ledmsg;
msg_t lcdcommands;

comuni webmos(ssid, pass, ip, dns, gateway, subnet, 5557); 

void setup(){
  Wire.begin(); //Het initialiseren van de Wire library en het joinen van de I2C bus.
  Serial.begin(74880); // baut = 74880 = baut of ESP8266 boot sequance

  initi2c();
  intiLEDS();

  webmos.connect();
}

void loop(){ 
//read analogue sensor values and convert them to uint16_t
  uint16_t value = LdrUitlezen();
  sendingDataLDR[1] = (uint8_t)(value >> 8); 
  sendingDataLDR[2] = (uint8_t)(value); 

  uint16_t value2 = DimmerUitlezen();
  sendingDataPOD[1] = (uint8_t)(value2 >> 8); 
  sendingDataPOD[2] = (uint8_t)(value2); 

  if(webmos.pollserver()){

    webmos.send_msg(0x51,sendingDataLDR);      
    webmos.send_msg(0x52,sendingDataPOD);

    while(webmos.pollclient()){
      lcdcommands = webmos.recv_msg(); //comp ID for LCD = 0x53    
      ledmsg = webmos.recv_msg();      //comp ID for LED = 0x54

      if(lcdcommands.data[2] == 1){
        BlinderingOmlaag();       
      }else if(lcdcommands.data[2] == 0){
        BlinderingOmhoog();   
      }

      RegelLichtsterkte(ledmsg.data[0], ledmsg.data[1], ledmsg.data[2]);  
    }
  }
}



void intiLEDS(){
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS); // Setup voor de LED strip.
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 500);
  FastLED.clear();
  FastLED.show();
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

void RegelBlindering(){ //niet nodig
  if (LdrUitlezen() < 500) //Deze waarden fluctueren zo hard nog.
    BlinderingOmlaag(); //Het is volgens de user stories de bedoeling dat de blindering omlaag gaat als het donker buiten is. Het zal ook wenselijk zijn dat bij te veel licht de blindering omlaag zal gaan.
  else
    BlinderingOmhoog();  
}

void RegelLichtsterkte(uint8_t R, uint8_t G, uint8_t B){ //niet nodig
  for (int i = 0; i < NUM_LEDS; i++ ) //Loop voor het regelen van de brightness, zoals het gewenst is in de user story.
  {
    leds[i] = CRGB(R, G, B); //CRGB code voor oranje licht.
    FastLED.show();
  }
}  

void RegelLichtshow(){
  if (DimmerUitlezen() > 0 && DimmerUitlezen() <= 300){
    leds[2] = CRGB::DarkKhaki;
    leds[1] = CRGB::DarkSalmon;
    leds[0] = CRGB::Gold;

    FastLED.setBrightness(35);
    FastLED.show();
  }else if(DimmerUitlezen() > 300 && DimmerUitlezen() <= 700){
    leds[2] = CRGB::Aqua;
    leds[1] = CRGB::Purple;
    leds[0] = CRGB::Red;

    FastLED.setBrightness(35);
    FastLED.show();
  }else if(DimmerUitlezen() > 700){
    leds[2] = CRGB::Orange;
    leds[1] = CRGB::DeepSkyBlue;
    leds[0] = CRGB::DarkGreen;

    FastLED.setBrightness(35);
    FastLED.show();
  }       
}

uint16_t LdrUitlezen(){
  Wire.requestFrom(0x36, 4); //Communicatie request, er is voor de LDR maar 2 byte aan data nodig. 
  uint16_t anIn0 = Wire.read() & 0x03;  // 1 byte uitlezen en deze opslaan in een analoge input waarde integer. 
  anIn0 = anIn0 << 8; // Bitshifting om te voorkomen dat nieuwe bits aan data de oude niet overschrijven. 
  anIn0 = anIn0 | Wire.read(); //Merging de waardes van beide bytes in één variabele. 
  return anIn0;
}

uint16_t DimmerUitlezen(){
  Wire.requestFrom(0x36, 4); //Communicatie request, voor de dimmer zijn 4 bytes nodig.
  Wire.read(); //read to advance to next byte this is for analogue 0 HIGH
  Wire.read(); //read to advance to next byte this is for analogue 0 LOW
  uint16_t anIn1 = Wire.read() & 0x03; //Analogue 1 HIGH // Door deze twee keer uit te lezen, wordt de waarde van de LDR overschreven met de waarde van de dimmer. 
  anIn1 = anIn1 << 8; //Opnieuw bitshiften om overschrijven te voorkomen. 
  anIn1 = anIn1 | Wire.read(); //merge with Analogue 1 LOW //Opnieuw mergen. 
  return anIn1;
} 

void BlinderingOmlaag(){
  Wire.beginTransmission(0x38); 
  Wire.write(byte(0x01));            
  Wire.write(byte(1<<4)); // Schrijft een 1 naar de positie van D04 op 0x38 van de WIB. Dit komt overeen met de LCD panel, die door de 1 nu zwart wordt. 
  Wire.endTransmission(); 
}

void BlinderingOmhoog(){
  Wire.beginTransmission(0x38); 
  Wire.write(byte(0x01));            
  Wire.write(byte(0<<4)); // Schrijft een 0 naar de positie van D04 op 0x38 van de WIB. Dit komt overeen met de LCD panel, die door de 1 nu doorzichtig wordt.     
  Wire.endTransmission(); 
}
