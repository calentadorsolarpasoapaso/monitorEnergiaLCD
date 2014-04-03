/*

  sonidoPC.cpp - Library for sound a PC speaker
  Created by José Miguel Jimenez
  Released into the public domain.

    */ 

#include <Arduino.h> 
#include "SonidoPC.h"


// set pin numbers:
// PIN 3 y 2
int  LedPinSonidoPC =  5;      // the number of the LED pin
//const int LedPinSonidoPC2 =  2;      // the number of the LED pin

// Variables will change:
int ledStateSonidoPC = 0;             // ledStateSonidoPC used to set the LED
//int ledStateSonidoPC2 = HIGH;             // ledStateSonidoPC used to set the LED
long previousMillisSonidoPC = 0;        // will store last time LED was updated
boolean subiendo=true;


// the follow variables is a long because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long intervalSonidoPC = 200;           // interval at which to blink (milliseconds)
int INCREMENTO=40; //Incremento

SonidoPC::SonidoPC() {
  // set the digital pin as output:
  // set the digital pin as output:
  pinMode(LedPinSonidoPC, OUTPUT);      
 // pinMode(LedPinSonidoPC2, OUTPUT); 
  analogWrite(LedPinSonidoPC,INCREMENTO);
  //digitalWrite(LedPinSonidoPC2,LOW); 
  subiendo=true;
}

void SonidoPC::setPinPWMSonido(int pinPWM){
  LedPinSonidoPC=pinPWM;
  pinMode(LedPinSonidoPC, OUTPUT);      
}

void SonidoPC::cambiar(){
    // if the LED is off turn it on and vice-versa:
    if (subiendo){
      ledStateSonidoPC += INCREMENTO;
    }
    else{
      ledStateSonidoPC -= INCREMENTO;
    }
    if(ledStateSonidoPC>(255-INCREMENTO)) subiendo=false;
    //Con pausa en el sonido para no resultar muy cansino
    if(ledStateSonidoPC<(-(INCREMENTO*3))) subiendo=true;

    int valorPWM=ledStateSonidoPC;
    if(valorPWM<0) valorPWM=0;
//    Serial.println(ledStateSonidoPC);
    // set the LED with the ledStateSonidoPC of the variable:
    analogWrite(LedPinSonidoPC, valorPWM);
    

}
void SonidoPC::silencio(){
  analogWrite(LedPinSonidoPC,0);
  //digitalWrite(LedPinSonidoPC2,LOW); 
}
void SonidoPC::sonar()
{
  // here is where you'd put code that needs to be running all the time.

  // check to see if it's time to blink the LED; that is, if the 
  // difference between the current time and last time you blinked 
  // the LED is bigger than the interval at which you want to 
  // blink the LED.
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillisSonidoPC > intervalSonidoPC) {
    // save the last time you blinked the LED 
    previousMillisSonidoPC = currentMillis;   

    cambiar();
    
  }
}



