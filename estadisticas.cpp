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
  watsHoraExportacion=0.00000;
  watsHoraExportacion_2=0.00000;
  watsHoraExportacion_3=0.00000;

  watsHoraImportacionAcumulado=0.00;

  watsHoraImportacion=0.00000;
  watsHoraImportacion_2=0.00000;
  watsHoraImportacion_3=0.00000;
  watsHoraImportacionAyer=0.00000;

  //TODO ELIMINAR
  horasAcumuladoExportacion=0.00;
  horasAcumuladoImportacion=0.00;
  
  msInicioReset=millis();
}

//Suma los watios acumulados en los segundos del parámetro. w/ms
void Estadisticas::sumaWatsHoraExportacion(long msExportacion, int watt)
{
  watsHoraExportacion += calcularConsumo(msExportacion,watt);
  horasAcumuladoExportacion += msExportacion/(3600000.0);
}


/*Suma los watios acumulados en los segundos del parámetro. w/ms
//Recibo los watss/hora de un periodo concreto de algunos milisegundos, tengo que pasar todo a milisegundos
//Ejm 300w/h en 500 ms serían (X/500)*1000*60*60
//Los watts de ese periodo serían (300/(60*60*1000))*500=0,04166666666666666666666666666667;

*/

float Estadisticas::calcularConsumo(long ms, int watt){
    float watiosParcial=0.0;
    watiosParcial=((watt*ms)/(3600000.0));
    return watiosParcial;    
}

void Estadisticas::sumaWatsHoraImportacion(long msImportacion, int watt)
{
  watsHoraImportacion =calcularConsumo(msImportacion,watt) + watsHoraImportacion;
  horasAcumuladoImportacion+= (msImportacion/(3600000.0));
  
  //Si los milisegundos iniciales del sistema son >24h, reseteamos
  if((millis()-msInicioReset)>86400000){
//   if((millis()-msInicioReset)>60000){
    watsHoraExportacion_3=watsHoraExportacion_2;
    watsHoraExportacion_2=watsHoraExportacion;
  
    watsHoraImportacion_3=watsHoraImportacion_2;
    watsHoraImportacion_2=watsHoraImportacionAyer;
    watsHoraImportacionAyer=watsHoraImportacion;

    watsHoraImportacionAcumulado+=watsHoraImportacion;
    watsHoraImportacionAyer=watsHoraImportacion;

    //Reseteamos los dos contadores
    watsHoraImportacion=0.00;
    watsHoraExportacion=0.0;
    diasCompletos++;

    msInicioReset=millis();
  }
}

//Para sacar el valor en wat/hora tenemos que multiplicar 
//ya que tenemos guardado ms/watio
//si tenemos por ejemplo 
//0,04166666666 en 500 ms tendremos que: 0,04166666666*500
String Estadisticas::getWatHoraExportados(){
  String texto="E:";
  char buffer[20];
//  float tiempoHoras=horasAcumuladoExportacion; //Horas que se ha exportado

  dtostrf(watsHoraExportacion, 6, 2, buffer);

  texto+=buffer;
  texto+="/";

  dtostrf(watsHoraExportacion_2, 4, 0, buffer);
  texto+=buffer;
  texto+="/";

  dtostrf(watsHoraExportacion_3, 4, 0, buffer);
  texto+=buffer;

  return texto;
}

String Estadisticas::getWatHoraImportados(){
  String texto="I:";
  char buffer[20];

  float tiempoHoras=horasAcumuladoImportacion;
  float wattHoras=watsHoraImportacion;

  dtostrf(wattHoras, 6, 2, buffer);

//  deblank(buffer);

  texto+=buffer;

  texto+="/";

  dtostrf(watsHoraImportacionAcumulado, 6, 0, buffer);
  texto+=buffer;
  texto+="W ";
  texto+=diasCompletos;
  texto+="D";
  

  return texto;
}

String Estadisticas::getWatHoraImportadosAyer(){
  String texto="H:";
  char buffer[20];

  dtostrf(watsHoraImportacionAyer, 4, 0, buffer);
  texto+=buffer;
  texto+="/";

  dtostrf(watsHoraImportacion_2, 4, 0, buffer);
  texto+=buffer;
  texto+="/";

  dtostrf(watsHoraImportacion_3, 4, 0, buffer);
  texto+=buffer;

  return texto;
}


char* Estadisticas::deblank(char* input)                                                  /* deblank accepts a char[] argument and returns a char[] */
{
    char *output=input;
    for (int i = 0, j = 0; i<strlen(input); i++,j++)                        /* Evaluate each character in the input */
    {
        if (input[i]!=' ')                                                  /* If the character is not a space */
            output[j]=input[i];                                             /* Copy that character to the output char[] */
        else
            j--;                                                            /* If it is a space then do not increment the output index (j), the next non-space will be entered at the current index */
    }
    return output;                                                          /* Return output char[]. Should have no spaces*/
}
