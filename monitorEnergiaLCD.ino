
#include "EmonLib.h"
#include "estadisticas.h"
#include <LiquidCrystal.h>
#include <LCDKeypad.h>
#include <Wire.h> 
#include <VirtualWire.h> 
#include <LiquidCrystal_I2C.h>

/* Antigua pantalla
LiquidCrystal lcd(8, 13, 9, 4, 5, 6, 7);
*/
//Addr: 0x3F, 20 chars & 4 lines
//Arduino NANO pines A4 y A5
LiquidCrystal_I2C lcd(0x27,20,4); 

char Sensor1CharMsg[8];

EnergyMonitor emon1;             // Create an instance
Estadisticas stats;

const uint8_t PIN_RADIO_FRECUENCIA=11;

//Posición A6 del rango
const uint8_t PIN_SONIDO=6;

boolean inyectando=false;
boolean errorVoltaje=false;

void setup()
{  
  Serial.begin(9600);

//Inicializamos la pantalla LCD.
  lcd.begin(20, 4);
  lcd.init(); 
  lcd.backlight();
  lcd.clear();
  digitalWrite(13, true); // Flash a light to show transmitting

  lcd.setCursor(0,3);
  lcd.print("Blocalentadorsolarpasoapaso");
  
//  emon1.voltage(2, 135, 0.1);  // Voltage: input pin, calibration, phase_shift
          //135->215
          //145->231
  emon1.voltage(2, 144, 0.1);  // Voltage: input pin, calibration, phase_shift
  emon1.current(1, 145);       // Current: input pin, calibration.
  
  emon1.setPinPWMSonido(PIN_SONIDO);

  stats.reset();
  
    // Initialise the IO and ISR
    vw_set_tx_pin(PIN_RADIO_FRECUENCIA);
    vw_set_ptt_inverted(true); // Required for DR3100
    vw_setup(2000);	 // Bits per sec}
}
void loop()
{
  long msCalculo=millis();
  //Sonará la alarma si hay inyección o error en voltaje
  emon1.calcVI(100,2000,inyectando||errorVoltaje);         // Calculate all. No.of half wavelengths (crossings), time-out
  
  float realPower       = emon1.realPower;        //extract Real Power into variable
  float apparentPower   = emon1.apparentPower;    //extract Apparent Power into variable
  float powerFActor     = emon1.powerFactor;      //extract Power Factor into Variable
  float supplyVoltage   = emon1.Vrms;             //extract Vrms into Variable
  float Irms            = emon1.Irms;             //extract Irms into Variable

//  realPower=-2;  

  Serial.println(supplyVoltage);
  Serial.println(realPower);
  
  //Si no nos llega voltaje, o es muy grande, alerta, el sistema no funciona
  errorVoltaje=hayErrorVoltaje(supplyVoltage);
  
  writeLCDValues(realPower,supplyVoltage);


  //TEST FUERZO REAL POWER
  if(realPower<0){
    inyectando=true;
    stats.sumaWatsHora((millis()-msCalculo)/1000,realPower);
   }
   else inyectando=false;
  
    String textomsg=String((int)realPower); 
    textomsg+= " ";
    textomsg+=millis(); 
    const char *msg=textomsg.c_str();
    Serial.println(textomsg);
  //Enviar consumo por radiofrecuencia
    //digitalWrite(13, tru e); // Flash a light to show transmitting
    vw_send((uint8_t *)msg, strlen(msg));
    vw_wait_tx(); // Wait until the whole message is gone
    //digitalWrite(13, false);
}

void writeLCDValues(int watios,int voltios){
  String textoWatios="Wats: ";
  String textoVoltios="V";
  String textoInyectando="!ALERTA! INYECTANDO";
  String textoErrorVoltaje="ALERTA!! VOLTAJE BAJO";
  String textoLimpio=    "                    ";
  // Limpiamos LCD
  //Esto tarda 2 segundos, lo elimino
//  lcd.clear();

  //Escribimos los watios en la primera fila  
  if(watios>4000 || watios < -4000){ 
    textoWatios= textoWatios + "ERROR"+ watios + "        ";
  }
  else{
      textoWatios= textoWatios + watios + "       " ;
  }
  lcd.setCursor(0,0);
  lcd.print(textoWatios);

  //Escribimos los voltios en la segunda fila
  
  if(hayErrorVoltaje(voltios)){
    lcd.setCursor(0,0);
    textoVoltios = "ERROR LECTURA VOLTAJE: " + voltios ;
    lcd.print(textoErrorVoltaje);
  }
  else{
    lcd.setCursor(16,0);
    textoVoltios = voltios + textoVoltios;
    lcd.print(textoVoltios);
  }
 
  
  lcd.setCursor(0,1);
  if(watios<0) {
    lcd.print(textoInyectando);
  }
  else {
    lcd.print(textoLimpio);
    lcd.setCursor(0,1);
    lcd.print(stats.getWatHoraInyectados());
  }
}


boolean hayErrorVoltaje(int supplyVoltage){

  boolean error;
  if(supplyVoltage<200 || supplyVoltage>250) {
      error=true;    
  }
  else error=false;
  
  return error;
}

