/**
 *
 * Pins:
 * Hardware SPI:
 * MISO -> 12
 * MOSI -> 11
 * SCK -> 13
 *
 * Configurable:
 * CE -> 8
 * CSN -> 7
 */
 
 //DHT pin 6
 //DS18S10 pin 5

#include "SPI.h"
#include "Mirf.h"
#include "nRF24L01.h"
#include "MirfHardwareSpiDriver.h"
#include <dht.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "LowPower.h"

#define ONE_WIRE_BUS 5
#define TEMPERATURE_PRECISION 12

dht DHT;

#define DHT11_PIN 6

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress insideThermometer, outsideThermometer;


  byte j;
  byte data[12];
  byte addr[8];
  float celsius;
  float celsius_out;
  float light;
  float voltage;
  int k;



// converts a float into a char 
// and sends it via nRF24L01
void transmit( float v)
{
  byte c; 
  char buf[10];
  
  dtostrf(v,9,3,buf);

  for( int i=3 ; i < 8 ; i++ ) //allagi gia to megethos tou float pou stelnetai
  { 
    c = buf[i];
    Mirf.send(&c);
    while( Mirf.isSending() ) ;
  }
}

// sends a string via the nRF24L01
void transmit(const char *string)
{
  byte c; 
  
  for( int i=0 ; string[i]!=0x00 ; i++ )
  { 
    c = string[i];
    Mirf.send(&c);
    while( Mirf.isSending() ) ;
  }
}

// send a CR/LF sequence via the nRF24L01
void transmitlf(void)
{
  byte c;
  
  c = '\r';
  Mirf.send(&c);
    while( Mirf.isSending() ) ;
  
  c = '\n';
  Mirf.send(&c);
    while( Mirf.isSending() ) ;
}

void setup()
{
  //Serial.begin(9600);
  
  sensors.begin();
  
  Serial.print("Locating devices...");
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");

   pinMode(4,OUTPUT);
   digitalWrite(4,LOW);
  
  Mirf.cePin = 9;
  Mirf.csnPin = 10;
  
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();   
  Mirf.setTADDR((byte *)"serv1");
  Mirf.payload = 1;
  Mirf.config();
  
  Serial.print("Parasite power is: ");
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");
  
  if (!sensors.getAddress(insideThermometer, 0)) Serial.println("Unable to find address for Device 0");
  if (!sensors.getAddress(outsideThermometer, 1)) Serial.println("Unable to find address for Device 1");  
  
  sensors.setResolution(insideThermometer, TEMPERATURE_PRECISION);
  sensors.setResolution(outsideThermometer, TEMPERATURE_PRECISION);
}

void loop() 
{
  sensors.requestTemperatures();
  delay(1000);
  
  celsius = sensors.getTempC(insideThermometer);
  celsius_out = sensors.getTempC(outsideThermometer);
  
  Serial.print("Inside: ");
  Serial.println(celsius);
  Serial.print("Outside: ");
  Serial.println(celsius_out);
  
 
  DHT.read11(DHT11_PIN);
  light = map(analogRead(A0),0,1023,0,100);
  voltage = float((analogRead(A1))*15.0 / 1023.0);
  
  transmit(DHT.humidity);
  transmit(":");;
  transmit(DHT.temperature);
  transmit(":");
  transmit(celsius_out);
  transmit(":");
//  transmit(celsius);
//  transmit(":");
  transmit(light);
  transmit(":");
  transmit(voltage);
  transmit("~");
 
  // ... just take your time
  Mirf.powerDown();
 for(k=1;k<=15;k++){ 
  //delay(10000);
  LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);  
 }
//  digitalWrite(4,1);
//  delay(50);
//  digitalWrite(4,0);
//  Serial.println("A");
}
