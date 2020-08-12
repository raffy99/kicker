//
// Reels01
//
// Description of the project
// Developed with [embedXcode](https://embedXcode.weebly.com)
//
// Author     Raffy
//        Raffy
//
// Date     17.08.19 23:37
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
#include <stdio.h>
#include <SoftwareSerial.h>

// Define structures and classes


// Define variables and constants
char c;
int reels[4];
const int digitalPins[4] = {3,4,5,6};
const int inputPins[4] = {7,8,9,12};
int team1 = 0, team2 = 0;

#define rxPin 10
#define txPin 11
SoftwareSerial btSerial(rxPin, txPin);

// Prototypes
// !!! Help: http://bit.ly/2l0ZhTa


// Utilities


// Functions
void knock(int pos, int value){
    String msg;
    
    if (value < 0){
        value = 10 + value;
    }
    for (int i=1; i<= value; i++){
        msg = "Position " + String(pos) + "(PIN: " + String(digitalPins[pos]) + "): knock" + i;     
        Serial.println(msg);           
        digitalWrite(digitalPins[pos], 1);
        delay(40);
        digitalWrite(digitalPins[pos], 0);        
        delay(80);
    }
}

void setReel(int pos, int value){
    int diff = value - reels[pos];
    reels[pos] = value;
    knock(pos, diff);
}

void initReel(int pos){
    String msg;
    boolean isNull = false;    
    msg = "Init Position:" + pos;
    Serial.println(msg);               
    while(!isNull){
      knock(pos, 1);
      isNull = checkNull(pos);      
    }
}

boolean checkNull(int pos) {
    delay(80);
    if ( digitalRead(inputPins[pos]) == LOW ){
      Serial.println("NUll \n");      
      return true;  
    }
    Serial.println("NICHT NUll \n");          
    return false;
}


// Add setup code
void setup()
{
    Serial.begin(9600);
    btSerial.begin(38400);

    pinMode(7, INPUT_PULLUP);
    pinMode(8, INPUT_PULLUP);
    pinMode(9, INPUT_PULLUP);
    pinMode(12, INPUT_PULLUP);    

    Serial.println("\n\n\n====== Reels01 ======\n\n");

    for (int i=0; i < (sizeof(reels)/sizeof(int)); i++){
        initReel(i);
    }
}

char rx_byte = 0;
String rx_str = "";
int int_team1, int_team2;

// Add loop code
void loop()
{
    if (btSerial.available() > 0){
        rx_byte = btSerial.read();       // get the character
        
        if (rx_byte != '\n') {
          rx_str += rx_byte;
        }
        else {
          Serial.println(rx_str);
          int ind1 = rx_str.indexOf(':');
          String satz = rx_str.substring(0, ind1);
          int ind2 = rx_str.indexOf(':', ind1 +1 );
          String team1 = rx_str.substring(ind1 +1, ind2);          
          int int_team1 = team1.toInt();
          int ind3 = rx_str.indexOf(':', ind2 +1);
          String team2 = rx_str.substring(ind2 +1, ind3);
          int int_team2 = team2.toInt();
          int ind4 = rx_str.indexOf(':', ind3 +1);
          String satzTeam1 = rx_str.substring(ind3 +1, ind4);
          int ind5 = rx_str.indexOf(':', ind4 +1);
          String satzTeam2 = rx_str.substring(ind4 +1, ind5);          
          int ind6 = rx_str.indexOf(':', ind5+1);
          String tore = rx_str.substring(ind5+1, ind6);        
          Serial.print(satz);
          Serial.print(":");
          Serial.print(team1);
          Serial.print(":");
          Serial.print(team2);            
          Serial.print(":");
          Serial.print(satzTeam1);            
          Serial.print(":");
          Serial.print(satzTeam2);                                
          Serial.print(":");
          Serial.print(tore);                      
          Serial.println("\n");
          rx_str = "";

          setReel(1, int_team1 %10);
          setReel(0, (int_team1 - reels[1]) /10);

          setReel(3, int_team2 %10);
          setReel(2, (int_team2 - reels[3]) /10);     
        }        
    }
}
