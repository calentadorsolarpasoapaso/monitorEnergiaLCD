
#include "EmonLib.h"             // Include Emon Library
#include <LiquidCrystal.h>
#include <LCDKeypad.h>

LiquidCrystal lcd(8, 13, 9, 4, 5, 6, 7);

EnergyMonitor emon1;             // Create an instance

boolean inyectando=false;

void setup()
{  
  Serial.begin(9600);

//Inicializamos la pantalla LCD.
  lcd.begin(16, 2);
  lcd.clear();
  
  emon1.voltage(2, 135, 0.1);  // Voltage: input pin, calibration, phase_shift
  emon1.current(1, 145);       // Current: input pin, calibration.
}

void loop()
{
  emon1.calcVI(100,2000,sonando);         // Calculate all. No.of half wavelengths (crossings), time-out
  //emon1.serialprint();           // Print out all variables (realpower, apparent power, Vrms, Irms, power factor)
  
  float realPower       = emon1.realPower;        //extract Real Power into variable
  float apparentPower   = emon1.apparentPower;    //extract Apparent Power into variable
  float powerFActor     = emon1.powerFactor;      //extract Power Factor into Variable
  float supplyVoltage   = emon1.Vrms;             //extract Vrms into Variable
  float Irms            = emon1.Irms;             //extract Irms into Variable
  
  writeLCDValues(realPower,supplyVoltage);


  
  if(realPower<0){
    inyectando=true;
   }
   else sonando=false;

}

void writeLCDValues(int watios,int voltios){
  String textoWatios="Wats: ";
  String textoVoltios="Volt: ";
  
  // Limpiamos LCD
  lcd.clear();
  
  
  //Escribimos los watios en la primera fila  
  if(watios>4000 || watios < -4000){ 
    textoWatios= textoWatios + "ERROR"+ watios;
  }
  else{
      textoWatios= textoWatios + watios ;
  }
  
  lcd.print(textoWatios);

  //Escribimos los voltios en la segunda fila
  lcd.setCursor(0,1);
  if(voltios>250){
    textoVoltios = textoVoltios +  "ERROR" ;
  }
  else{
    textoVoltios = textoVoltios +  voltios;
  }
  lcd.print(textoVoltios);

}
