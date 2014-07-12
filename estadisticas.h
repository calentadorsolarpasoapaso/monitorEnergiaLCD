/*
  estadisticas.cpp - Library for calentadorsolarpasoapaso.blogspot.com
  Created by Jose Miguel Jiménez Villanueva April 2014
  GNU GPL
*/

#ifndef Estadisticas_h
#define Estadisticas_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif

class Estadisticas
{
  public:

    void reset();
    void sumaWatsHoraExportacion(long msExportacion, int valorWatt);
    void sumaWatsHoraImportacion(long msImportacion, int valorWatt);
//    void setHoraInyeccion(int _inPinI, double _ICAL);
    String getWatHoraExportados();
    String getWatHoraImportados();
    String getWatHoraImportadosAyer();
    
  private:
    int diasCompletos;
    float watsHoraImportacion;
    float watsHoraImportacionAcumulado;
    float watsHoraImportacionAyer,watsHoraImportacion_2,watsHoraImportacion_3;
    float watsHoraExportacion,watsHoraExportacion_2,watsHoraExportacion_3;
    float horasAcumuladoImportacion;
    float horasAcumuladoExportacion;
    long msInicioReset;    
    float calcularConsumo(long ms, int watt);
    char* deblank(char* input);

};

#endif
