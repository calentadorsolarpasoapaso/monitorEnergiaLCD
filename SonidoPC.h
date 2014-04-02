/*
  SonidoPC.h - sonido
*/

#ifndef SonidoPC_h
#define SonidoPC_h

class SonidoPC
{
  public: SonidoPC();
 void sonar();
 void silencio(); 
 void cambiar();
 void setPinPWMSonido(int pinPWM);

 };
#endif
