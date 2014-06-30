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
  watsHoraExportacion=0;
  msAcumuladoExportacion=0;
  watsHoraImportacion=0;
  msAcumuladoImportacion=0;
}

//Suma los watios acumulados en los segundos del parámetro. w/ms
void Estadisticas::sumaWatsHoraExportacion(long msExportacion, int watt)
{
  watsHoraExportacion += calcularConsumo(msExportacion,watt);
  msAcumuladoExportacion += msExportacion;
}


/*Suma los watios acumulados en los segundos del parámetro. w/ms
//Recibo los watss/hora de un periodo concreto de algunos milisegundos, tengo que pasar todo a milisegundos
//Ejm 300w/h en 500 ms serían (X/500)*1000*60*60
//Los watts de ese periodo serían (300/(60*60*1000))*500=0,04166666666666666666666666666667;

*/

float Estadisticas::calcularConsumo(long ms, int watt){
    float watiosParcial=0;
    watiosParcial=((watt*ms)/(60*60*1000));
    return watiosParcial;    
}

void Estadisticas::sumaWatsHoraImportacion(long msImportacion, int watt)
{
  watsHoraImportacion+=calcularConsumo(msImportacion,watt);
  msAcumuladoImportacion+=msImportacion;
}

//Para sacar el valor en wat/hora tenemos que multiplicar 
//ya que tenemos guardado ms/watio
//si tenemos por ejemplo 
//0,04166666666 en 500 ms tendremos que: 0,04166666666*500
String Estadisticas::getWatHoraExportados(){
  String texto="EXP: ";
  char buffer[20];
  float tiempoHoras=msAcumuladoExportacion/(1000*60*60); //Horas que se ha exportado
  float wattHoras=watsHoraExportacion;

  dtostrf(wattHoras, 5, 2, buffer);

  texto+=buffer;
  texto+=" ";

  dtostrf(tiempoHoras, 4, 1, buffer);

  texto+=buffer;
  texto+=" H";
  return texto;
}

String Estadisticas::getWatHoraImportados(){
  String texto="IMP: ";
  char buffer[20];

  float tiempoHoras=msAcumuladoImportacion/(1000*60*60);
  float wattHoras=watsHoraImportacion;

  dtostrf(wattHoras, 5, 2, buffer);

  texto+=buffer;
  texto+=" ";

  dtostrf(tiempoHoras, 4, 1, buffer);

  texto+=buffer;
  texto+=" H";
  return texto;
}

