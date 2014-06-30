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
    
  private:
    long watsHoraImportacion;
    long watsHoraExportacion;
    long msAcumuladoImportacion;
    long msAcumuladoExportacion;
    
    float calcularConsumo(long ms, int watt);

};

#endif
