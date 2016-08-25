/**
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

#include "SPI.h"
#include "Mirf.h"
#include "nRF24L01.h"
#include "MirfHardwareSpiDriver.h"
#include <string.h>
//#include "I2C_Anything.h"
//#include <Wire.h>

String message;
//String p, i;
char *tmp;
int i = 0;
float Data[6];

//const byte MY_ADDRESS = 0x16;

//volatile boolean haveData = false;
//volatile float fnum;
//volatile long foo;

void setup(){
  Serial.begin(9600);
  //Wire.begin (MY_ADDRESS);
  //Wire.onReceive (receiveEvent);
  //Wire.onRequest (requestEvent);  // interrupt handler for when data is wanted
  
  Mirf.cePin = 9;
  Mirf.csnPin = 10;
  
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();

  Mirf.setRADDR((byte *)"serv1");
  
  Mirf.payload = 1;

  Mirf.config();
  //Serial.println("hello");
}

void loop(){
   byte c; 
   
  // is there any data pending? 
  if(Mirf.dataReady())
  {
     // well, get it
     Mirf.getData(&c);

    // ... and write it out to the PC
     char letter = char(c);

       if(letter != '~')
       { 
         message = String(message + letter);
       }
       
       
     
     
     else 
     {
        tmp = strtok(&message[0], ":");
        while (tmp) 
          {
           Data[i] = atof(tmp);
           tmp = strtok(NULL, ":");
           i++;
           }
           i=0;
       //Serial.write(0xFF);
//       Serial.print(sizeof(Data));
//       Serial.print(",");
       Serial.print(Data[0]);
       Serial.print(",");
       Serial.print(Data[1]);
       Serial.print(",");
       Serial.print(Data[2]);
       Serial.print(",");
       Serial.print(Data[3]);
       Serial.print(",");
       Serial.print(Data[4]);
       Serial.print("~");
       
       //Serial.println();
       message = "";
     }
  }
  //Mirf.powerDown(); 
  //delay(2000);
} 


//// called by interrupt service routine when incoming data arrives
//void receiveEvent (int howMany)
// {
// if (howMany >= (sizeof fnum) + (sizeof foo))
//   {
//   I2C_readAnything (fnum);   
//   I2C_readAnything (foo);   
//   haveData = true;     
//   }  // end if have enough data
// }  // end of receiveEvent
// 
// 
// void requestEvent ()
//  {
//  I2C_writeAnything (Data);   // send response
//  }  // end of requestEvent
