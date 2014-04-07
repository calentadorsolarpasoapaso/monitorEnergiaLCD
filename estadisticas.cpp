/*
  estadisticas.cpp - Library for calentadorsolarpasoapaso.blogspot.com
  Created by Jose Miguel Jiménez Villanueva April 2014
  GNU GPL
*/

//#include "WProgram.h" un-comment for use on older versions of Arduino IDE
#include "Estadisticas.h"

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else
 
#include "WProgram.h"

#endif


void Estadisticas::reset(){
  watsHora=0;
  segundosAcumulado=0;
}

//Suma los watios acumulados en los segundos del parámetro. w/ms
void Estadisticas::sumaWatsHora(int segundos, int watt)
{
  watsHora+=(segundos*watt);
  segundosAcumulado+=segundos;
}

String Estadisticas::getWatHoraInyectados(){
  String texto="EXP: ";
  char buffer[20];
  float tiempoHoras=segundosAcumulado/(60*60);
  //float wattHoras=watsHora/(60);
//  float tiempoHoras=segundosAcumulado;

  float wattHoras=watsHora/(60*60); //ojo son minutos

  dtostrf(wattHoras, 5, 2, buffer);

  texto+=buffer;
  texto+=" ";

  dtostrf(tiempoHoras, 4, 1, buffer);

  texto+=buffer;
  texto+=" H";
  return texto;
}

