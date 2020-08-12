//
// Kicker01
//
// Description of the project
// Developed with [embedXcode](https://embedXcode.weebly.com)
//
// Author     Raffy
//        Raffy
//
// Date     17.08.19 20:51
// Version    <#version#>
//
// Copyright  © Raffy, 2019
// Licence    <#licence#>
//
// See         ReadMe.txt for references
//


// Core library for code-sense - IDE-based
// !!! Help: http://bit.ly/2AdU7cu
#if defined(ENERGIA) // LaunchPad specific
#include "Energia.h"
#elif defined(TEENSYDUINO) // Teensy specific
#include "Arduino.h"
#elif defined(ESP8266) // ESP8266 specific
#include "Arduino.h"
#elif defined(ARDUINO) // Arduino 1.8 specific
#include "Arduino.h"
#else // error
#error Platform not defined
#endif // end IDE

// Set parameters


// Include application, user and local libraries
// !!! Help http://bit.ly/2CL22Qp
#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>
#include <stdio.h>
#include <RBD_Button.h>

// Define structures and classes
typedef struct{
    unsigned int t1: 7;
    unsigned int t2: 7;
    unsigned int satz: 2;
    unsigned int satzTeam1: 2;
    unsigned int satzTeam2: 2;
} spielstand;

typedef struct{
    uint8_t tore;
    spielstand *stand;
} match;


// Define variables and constants
//bluetooth
#define rxPin 10
#define txPin 11

#define ledPin 3


char c;
String str_c = "";
spielstand info[212] = {0, 0, 1, 0, 0};
match game = {0, info};
SoftwareSerial btSerial(rxPin, txPin);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(12, ledPin, NEO_GRB + NEO_KHZ800);

uint32_t red = strip.Color(255, 0, 0);
uint32_t lightred = strip.Color(75, 0, 0);
uint32_t blue = strip.Color(0, 0, 255);
uint32_t lightblue = strip.Color(0, 0, 75);
uint32_t green= strip.Color(0, 255, 0);
uint32_t lightgreen= strip.Color(0, 125, 0);
uint32_t black= strip.Color(0, 0, 0);
uint32_t white= strip.Color(200, 200, 200);
uint32_t flash= strip.Color(255, 255, 255);

const uint8_t ledOrder[2][6] = {{0,1,2,3,4,5},{11,10,9,8,7,6}};
const uint32_t switchColor[4] = {red, blue, lightred, lightblue};

RBD::Button button1(7);
RBD::Button button2(8);
RBD::Button button3(9);

// Prototypes
// !!! Help: http://bit.ly/2l0ZhTa


// Utilities
#define ArrayCount(array) (sizeof array / sizeof array[0])


// Input a value 0 to 21
// to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
    WheelPos = 255 - WheelPos;
    if (WheelPos < 85) {
        return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
    }
    if (WheelPos < 170) {
        WheelPos -= 85;
        return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
    WheelPos -= 170;
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void flashLed(int ledPos, uint32_t color) {
    for (int r = 5; r < 16; r++) {
        strip.setPixelColor(ledPos, color);
        strip.show();
        delay(20 + r * 3);
        strip.setPixelColor(ledPos, black);
        strip.show();
        delay(10 + r * 3);
    }
}

void setLed(int ledPos, uint32_t color) {
    strip.setPixelColor(ledPos, color);
    strip.show();
}

void setSatzLed(int ledPos) {
    setLed(ledPos, green);
}

void initLeds(){
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, black);
    }
    strip.show();
}

void colorWipe(uint32_t c, uint8_t wait) {
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, c);
        strip.show();
        delay(wait);
    }
}

void rainbow(uint8_t wait) {
    uint16_t i, j;
    
    for (j = 0; j < 256; j++) {
        for (i = 0; i < strip.numPixels(); i++) {
            strip.setPixelColor(i, Wheel((i + j) & 255));
        }
        strip.show();
        delay(wait);
    }
}

void rainbowCycle(uint8_t wait) {
    uint16_t i, j;
    
    for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
        for (i = 0; i < strip.numPixels(); i++) {
            strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
        }
        strip.show();
        delay(wait);
    }
}

// Functions
void zeigeSpielStand(match *game, String schalter){
    String msg;
    uint8_t schalterInt = (uint8_t) schalter.toInt() - 1;
    
    for (uint8_t i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, black);
    }
    
    if (game->stand[game->tore].satz %2 != 0 ) {
        msg = String(game->stand[game->tore].satz) + ":" +
        String(game->stand[game->tore].t1) + ":" +
        String(game->stand[game->tore].t2) + ":" +
        game->stand[game->tore].satzTeam1 + ":" +
        game->stand[game->tore].satzTeam2 + ":" +
        game->tore;
    }else{
        msg = String(game->stand[game->tore].satz) + ":" +
        String(game->stand[game->tore].t2) + ":" +
        String(game->stand[game->tore].t1) + ":" +
        game->stand[game->tore].satzTeam1 + ":" +
        game->stand[game->tore].satzTeam2 + ":" +
        game->tore;
    }

    //Begin LED 
    if (game->stand[game->tore].t1 >= 6 ) {
        for (uint8_t j = 0; j < 6; j++){
            setLed(ledOrder[game->stand[game->tore].satz %2 ][j], switchColor[(game->stand[game->tore].satz %2) +2]);
        }
    }
    for (uint8_t j = 0; j < game->stand[game->tore].t1 %6; j++) {
        setLed(ledOrder[game->stand[game->tore].satz %2 ][j], switchColor[game->stand[game->tore].satz %2]);
    }
    
    if (game->stand[game->tore].t2 >= 6 ) {
        for (uint8_t j = 0; j < 6; j++){
            setLed(ledOrder[(game->stand[game->tore].satz+1) %2 ][j], switchColor[((game->stand[game->tore].satz+1) %2) +2]);
        }
    }
    for (uint8_t j = 0; j < game->stand[game->tore].t2 %6; j++) {
        setLed(ledOrder[(game->stand[game->tore].satz+1) %2 ][j], switchColor[(game->stand[game->tore].satz+1) %2]);
    }
    if (game->stand[game->tore].satzTeam1 > 0){
        setSatzLed(ledOrder[(game->stand[game->tore].satz) %2 ][5]);
    }
    if (game->stand[game->tore].satzTeam2 > 0){
        setSatzLed(ledOrder[(game->stand[game->tore].satz+1) %2 ][5]);
    }
    strip.show();
    
    Serial.println(msg);
    btSerial.println(msg);
}

void torZurueck(match *game) {
    game->stand[game->tore].satz = 0;
    game->stand[game->tore].satzTeam1 = 0;
    game->stand[game->tore].satzTeam2 = 0;
    game->tore--;
    zeigeSpielStand(game, " ");
}

void lastChance(match *game){
  long cTime = millis();
  long nextTime = cTime + 7000;
  float interval = 90;
  long prevTime = 0;
  int ledcount = 0;
  boolean leddir = false;  
  uint32_t color = green;
  uint32_t colorlight = lightgreen;  

  game->stand[game->tore].satz--;
  Serial.println(game->stand[game->tore].satz);
  if (game->stand[game->tore].satzTeam1 < game->stand[game->tore].satzTeam2) {
    if (game->stand[game->tore].satz %2 != 0 ){    
      color = switchColor[0];
      colorlight = switchColor[2];    
    }else{
      color = switchColor[1];
      colorlight = switchColor[3];    
    }    
  }else{
    if (game->stand[game->tore].satz %2  != 0) {    
      color = switchColor[1];
      colorlight = switchColor[3];    
    }else{
      color = switchColor[0];
      colorlight = switchColor[2];          
    }        
  }

  while (nextTime - cTime > 0) {
    if(button3.onReleased()) {
        Serial.println("Button3 Released");
        torZurueck(game);     
        cTime = nextTime;
    }else{ 
        if (cTime - prevTime >= interval) {
            interval = interval*0.985;
            if (interval < 25) interval = 25;
            prevTime = cTime;              
            if ( !leddir && ledcount < 11 ){
                ledcount++;
                if (ledcount == 11){
                 leddir = true;            
                }           
            }else{
                ledcount--;
                if (ledcount == 0){
                 leddir = false;            
                }           
            }
    
            strip.clear();
            if (leddir){
              strip.setPixelColor(ledcount +1, colorlight);                            
            }else{
              strip.setPixelColor(ledcount -1, colorlight);                            
            }
            strip.setPixelColor(ledcount, color);  
            strip.show();                  
        }
        cTime = millis();            
    }    
  }
  strip.fill(color);    
  
}

static void torTeam1(match *game, uint8_t schalterInt) {
    game->stand[game->tore].t1 = game->stand[game->tore-1].t1 + 1;
    game->stand[game->tore].t2 = game->stand[game->tore-1].t2;
}

static void torTeam2(match *game, uint8_t schalterInt) {
    game->stand[game->tore].t1 = game->stand[game->tore-1].t1;
    game->stand[game->tore].t2 = game->stand[game->tore-1].t2 + 1;
}

void  tor(String schalter, match *game) {
    uint8_t schalterInt = (uint8_t) schalter.toInt() - 1;
    if (schalter == "3") {
        if (game->tore > 0) {
            torZurueck(game);
        }
        return;
    }
    game->tore++;
    if (game->stand[game->tore-1].satz %2 != 0 ) {
        if (schalter == "1"){
            torTeam1(game, schalterInt);
            flashLed(ledOrder[1][(game->stand[game->tore].t1-1) %6], blue);            
        }else {
            torTeam2(game, schalterInt);
            flashLed(ledOrder[0][(game->stand[game->tore].t2-1) %6], red);                        
        }
    }else {
        if (schalter == "1"){
            torTeam2(game, schalterInt);
            flashLed(ledOrder[1][(game->stand[game->tore].t2-1) %6], blue);                        
        }else {
            torTeam1(game, schalterInt);
            flashLed(ledOrder[0][(game->stand[game->tore].t1-1) %6], red);                        
        }
    }
    game->stand[game->tore].satz = game->stand[game->tore-1].satz;
    game->stand[game->tore].satzTeam1 = game->stand[game->tore-1].satzTeam1;
    game->stand[game->tore].satzTeam2 = game->stand[game->tore-1].satzTeam2;
    zeigeSpielStand(game, schalter);
    
    if (game->stand[game->tore].t1 >= 6 || game->stand[game->tore].t2 >= 6){
        if (game->stand[game->tore].satz < 3) {
            game->stand[game->tore].satz++;
            if (game->stand[game->tore].t1 >= 6) {
                game->stand[game->tore].satzTeam1++;
            }else{
                game->stand[game->tore].satzTeam2++;
            }
            game->stand[game->tore].t1 = 0;
            game->stand[game->tore].t2 = 0;
            
            if (game->stand[game->tore].satzTeam1 < 2 && game->stand[game->tore].satzTeam2 < 2) {
                Serial.println("\n-----Seitenwechsel-----\n\n");
                initLeds();
                zeigeSpielStand(game, schalter);
            }
        }else{
            if ( (game->stand[game->tore].t1 - game->stand[game->tore].t2) >= 2){
                game->stand[game->tore].satzTeam1++;
                game->stand[game->tore].satz++;
            }
            if ( (game->stand[game->tore].t2 - game->stand[game->tore].t1) >= 2){
                game->stand[game->tore].satzTeam2++;
                game->stand[game->tore].satz++;
            }
        }
    }
}

static void newMatch() {
    game = {0, info};
    initLeds();
    Serial.println("\n\n\n====== New Game ======\n\n");        
    zeigeSpielStand(&game, " ");                            
}


// Add setup code
void setup()
{
    Serial.begin(9600);
    btSerial.begin(38400);
    
    pinMode(7, INPUT_PULLUP);
    pinMode(8, INPUT_PULLUP);
    pinMode(9, INPUT_PULLUP);
    
    strip.begin();
    rainbow(5);
    rainbowCycle(0);
    initLeds();
    button1.setDebounceTimeout(500);
    button2.setDebounceTimeout(500);
    button3.setDebounceTimeout(500);
    button1.invertReading();
    button2.invertReading();
    button3.invertReading();
    Serial.println("\n\n\n====== New Game ======\n\n");
    //lastChance(&game);
}

// Add loop code
void loop()
{
    if (Serial.available() > 0) {
        c = Serial.read();
        if (c != '\n') {
            str_c += c;
        }else {
            if (str_c == "1" || str_c == "2" || str_c == "3"){
                tor(str_c, &game);
            }else {
                btSerial.println(str_c);
                //zeigeSpielStand(&game, c);
            }
            str_c = "";
        }
    }
    if(button1.onReleased()) {
        Serial.println("Button1 Released");
        tor("1", &game);
    }
    if(button2.onReleased()) {
        Serial.println("Button2 Released");
        tor("2", &game);
    }
    if(button3.onReleased()) {
        Serial.println("Button3 Released");
        tor("3", &game);
        //lastChance(&game);        
    }
    
    if (game.stand[game.tore].satz > 3 || game.stand[game.tore].satzTeam1 >=2 || game.stand[game.tore].satzTeam2 >= 2) {
      lastChance(&game);   
      if (game.stand[game.tore].satz > 3 || game.stand[game.tore].satzTeam1 >=2 || game.stand[game.tore].satzTeam2 >= 2) {
          Serial.println("\n======Siegerehrung=====\n\n");  
          rainbow(1);
          rainbowCycle(1);        
          rainbow(2);        
          newMatch();          
      }         
    }       
}
