/*
  Emon.cpp - Library for openenergymonitor
  Created by Trystan Lea, April 27 2010
  GNU GPL
*/

//#include "WProgram.h" un-comment for use on older versions of Arduino IDE
#include "EmonLib.h"
#include "SonidoPC.h"

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else
 
#include "WProgram.h"

#endif


SonidoPC sonido;



//--------------------------------------------------------------------------------------
// Sets the pins to be used for voltage and current sensors
//--------------------------------------------------------------------------------------
void EnergyMonitor::voltage(int _inPinV, double _VCAL, double _PHASECAL)
{
   inPinV = _inPinV;
   VCAL = _VCAL;
   PHASECAL = _PHASECAL;
}

void EnergyMonitor::current(int _inPinI, double _ICAL)
{
   inPinI = _inPinI;
   ICAL = _ICAL;
}

//--------------------------------------------------------------------------------------
// Sets the pins to be used for voltage and current sensors based on emontx pin map
//--------------------------------------------------------------------------------------
void EnergyMonitor::voltageTX(double _VCAL, double _PHASECAL)
{
   inPinV = 2;
   VCAL = _VCAL;
   PHASECAL = _PHASECAL;
}

void EnergyMonitor::currentTX(int _channel, double _ICAL)
{
   if (_channel == 1) inPinI = 3;
   if (_channel == 2) inPinI = 0;
   if (_channel == 3) inPinI = 1;
   ICAL = _ICAL;
}

  void EnergyMonitor::setPinPWMSonido(int PIN_SONIDO){
      sonido.setPinPWMSonido(PIN_SONIDO);
  }

//--------------------------------------------------------------------------------------
// emon_calc procedure
// Calculates realPower,apparentPower,powerFactor,Vrms,Irms,kwh increment
// From a sample window of the mains AC voltage and current.
// The Sample window length is defined by the number of half wavelengths or crossings we choose to measure.
//--------------------------------------------------------------------------------------
void EnergyMonitor::calcVI(int crossings, int timeout,boolean sonando)
{
   #if defined emonTxV3
	int SUPPLYVOLTAGE=3300;
   #else 
	int SUPPLYVOLTAGE = readVcc();
   #endif
 //Serial.println(SUPPLYVOLTAGE);

  int crossCount = 0;                             //Used to measure number of times threshold is crossed.
  int numberOfSamples = 0;                        //This is now incremented  

  //-------------------------------------------------------------------------------------------------------------------------
  // 1) Waits for the waveform to be close to 'zero' (500 adc) part in sin curve.
  //-------------------------------------------------------------------------------------------------------------------------
  boolean st=false;                                  //an indicator to exit the while loop

  unsigned long start = millis();    //millis()-start makes sure it doesnt get stuck in the loop if there is an error.
  
  unsigned long maxV=0;
  unsigned long minV=1024;
  
  while(st==false)                                   //the while loop...
  {
     startV = analogRead(inPinV);     //using the voltage waveform
     if(maxV<startV) maxV=startV;
     if(minV>startV) minV=startV;
     
     //if ((startV < 550) && (startV > 440)) st=true;  //check its within range
     if ((millis()-start)>timeout) st = true;
     
     if(sonando){
      sonido.sonar();
    } 
  }
  /*
  Serial.print(maxV);
  Serial.print(" ");
  Serial.print(minV);
  Serial.print(" ");
  Serial.println(startV);
  */
  //Buscamos el punto medio
  unsigned long medio=(maxV-minV)/2;

  st=false;
  start = millis();
  while(st==false)                                   //the while loop...
  {
     startV = analogRead(inPinV);     //using the voltage waveform
        //Serial.print("ValorV ");
         //Serial.println(startV);
         if ((startV > (medio - 50)) && (startV < (medio + 50))){
           //Serial.println("Encontrado");
           st=true;  //check its within range
     }
     if ((millis()-start)>500) st = true;
     
     if(sonando){
      sonido.sonar();
    } 
  }

  //-------------------------------------------------------------------------------------------------------------------------
  // 2) Main measurment loop
  //------------------------------------------------------------------------------------------------------------------------- 
  start = millis(); 

  while ((crossCount < crossings) && ((millis()-start)<timeout)) 
  {
    numberOfSamples++;                            //Count number of times looped.

    lastSampleV=sampleV;                          //Used for digital high pass filter
    lastSampleI=sampleI;                          //Used for digital high pass filter
    
    lastFilteredV = filteredV;                    //Used for offset removal
    lastFilteredI = filteredI;                    //Used for offset removal   
    
    //-----------------------------------------------------------------------------
    // A) Read in raw voltage and current samples
    //-----------------------------------------------------------------------------
    sampleV = analogRead(inPinV);                 //Read in raw voltage signal
    sampleI = analogRead(inPinI);                 //Read in raw current signal

    //-----------------------------------------------------------------------------
    // B) Apply digital high pass filters to remove 2.5V DC offset (centered on 0V).
    //-----------------------------------------------------------------------------
    filteredV = 0.996*(lastFilteredV+(sampleV-lastSampleV));
    filteredI = 0.996*(lastFilteredI+(sampleI-lastSampleI));
   
    //-----------------------------------------------------------------------------
    // C) Root-mean-square method voltage
    //-----------------------------------------------------------------------------  
    sqV= filteredV * filteredV;                 //1) square voltage values
    sumV += sqV;                                //2) sum
    
    //-----------------------------------------------------------------------------
    // D) Root-mean-square method current
    //-----------------------------------------------------------------------------   
    sqI = filteredI * filteredI;                //1) square current values
    sumI += sqI;                                //2) sum 
    
    //-----------------------------------------------------------------------------
    // E) Phase calibration
    //-----------------------------------------------------------------------------
    phaseShiftedV = lastFilteredV + PHASECAL * (filteredV - lastFilteredV); 
    
    //-----------------------------------------------------------------------------
    // F) Instantaneous power calc
    //-----------------------------------------------------------------------------   
    instP = phaseShiftedV * filteredI;          //Instantaneous Power
    sumP +=instP;                               //Sum  
    
    //-----------------------------------------------------------------------------
    // G) Find the number of times the voltage has crossed the initial voltage
    //    - every 2 crosses we will have sampled 1 wavelength 
    //    - so this method allows us to sample an integer number of half wavelengths which increases accuracy
    //-----------------------------------------------------------------------------       
    lastVCross = checkVCross;                     
    if (sampleV > startV) checkVCross = true; 
                     else checkVCross = false;
    if (numberOfSamples==1) lastVCross = checkVCross;                  
                     
    if (lastVCross != checkVCross) crossCount++;
    
    if(sonando){
      sonido.sonar();
    } 
  }
//  Serial.print(crossCount);
//  Serial.print(" ");  
//  Serial.println(numberOfSamples);
 
  //-------------------------------------------------------------------------------------------------------------------------
  // 3) Post loop calculations
  //------------------------------------------------------------------------------------------------------------------------- 
  //Calculation of the root of the mean of the voltage and current squared (rms)
  //Calibration coeficients applied. 
  
double V_RATIO = VCAL *((SUPPLYVOLTAGE/1000.0) / 1023.0);
  //double V_RATIO = 2;
  Vrms = V_RATIO * sqrt(sumV / numberOfSamples); 
  
  double I_RATIO = ICAL *((SUPPLYVOLTAGE/1000.0) / 1023.0);
//  double I_RATIO=-0.08;
  Irms = I_RATIO * sqrt(sumI / numberOfSamples); 

  //Calculation power values
  realPower = V_RATIO * I_RATIO * sumP / numberOfSamples;
  apparentPower = Vrms * Irms;
  powerFactor=realPower / apparentPower;



//Output to serial
/*Serial.print(realPower);
Serial.print(' ');
Serial.print(apparentPower);
Serial.print(' ');
Serial.print(powerFactor);

String t=" RMS: ";
Serial.print(t);
Serial.print(Vrms);
Serial.print(' ');
Serial.print(Irms);
Serial.print(' ');
Serial.print(V_RATIO);
Serial.print(' ');
Serial.println(I_RATIO);
*/
  //Reset accumulators
  sumV = 0;
  sumI = 0;
  sumP = 0;
//--------------------------------------------------------------------------------------       
}

//--------------------------------------------------------------------------------------
double EnergyMonitor::calcIrms(int NUMBER_OF_SAMPLES)
{
  
   #if defined emonTxV3
	int SUPPLYVOLTAGE=3300;
   #else 
	int SUPPLYVOLTAGE = readVcc();
   #endif

  
  for (int n = 0; n < NUMBER_OF_SAMPLES; n++)
  {
    lastSampleI = sampleI;
    sampleI = analogRead(inPinI);
    lastFilteredI = filteredI;
    filteredI = 0.996*(lastFilteredI+sampleI-lastSampleI);

    // Root-mean-square method current
    // 1) square current values
    sqI = filteredI * filteredI;
    // 2) sum 
    sumI += sqI;
  }

  double I_RATIO = ICAL *((SUPPLYVOLTAGE/1000.0) / 1023.0);
  Irms = I_RATIO * sqrt(sumI / NUMBER_OF_SAMPLES); 

  //Reset accumulators
  sumI = 0;
//--------------------------------------------------------------------------------------       
 
  return Irms;
}

void EnergyMonitor::serialprint()
{/*
    Serial.print(realPower);
    Serial.print(' ');
    Serial.print(apparentPower);
    Serial.print(' ');
    Serial.print(Vrms);
    Serial.print(' ');
    Serial.print(Irms);
    Serial.print(' ');
    Serial.print(powerFactor);
    Serial.println(' ');
    delay(100); 
    */
}

//thanks to http://hacking.majenko.co.uk/making-accurate-adc-readings-on-arduino
//and Jérôme who alerted us to http://provideyourown.com/2012/secret-arduino-voltmeter-measure-battery-voltage/

long EnergyMonitor::readVcc() {
  long result;
  
  //not used on emonTx V3 - as Vcc is always 3.3V - eliminates bandgap error and need for calibration http://harizanov.com/2013/09/thoughts-on-avr-adc-accuracy/

  #if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328__) || defined (__AVR_ATmega328P__)
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);  
  #elif defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  ADCSRB &= ~_BV(MUX5);   // Without this the function always returns -1 on the ATmega2560 http://openenergymonitor.org/emon/node/2253#comment-11432
  #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
  ADMUX = _BV(MUX5) | _BV(MUX0);
  #elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
  ADMUX = _BV(MUX3) | _BV(MUX2);
	
  #endif


  #if defined(__AVR__) 
  delay(2);                                        // Wait for Vref to settle
  ADCSRA |= _BV(ADSC);                             // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = 1126400L / result;                     //1100mV*1024 ADC steps http://openenergymonitor.org/emon/node/1186
  return result;
 #elif defined(__arm__)
  return (3300);                                  //Arduino Due
 #else 
  return (3300);                                  //Guess that other un-supported architectures will be running a 3.3V!
 #endif
}

