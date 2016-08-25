#include <SPI.h>
#include <Ethernet.h>
#include <Dns.h>
#include <HttpClient.h>
#include <Xively.h>
#include "SPI.h"
#include "Mirf.h"
#include "nRF24L01.h"
#include "MirfHardwareSpiDriver.h"
#include <string.h>

// MAC address for your Ethernet shield
byte mac[] = { 0xB8, 0x88, 0xE3, 0x06, 0x15, 0x28 };
//byte ip[] = { 192, 168, 1, 180 }; 
byte ip[] = { 192, 168, 1, 3 };
byte dnsIP[] = { 192, 168, 1, 1 };
byte gateway[] = { 192, 168, 1, 1 };
byte subnet[] = { 255, 255, 255, 0 };
//mac, ip, dns, gateway, subnet

// Your Xively key to let you upload data
//char xivelyKey[] = "GrhzMfngXCvKnIZD4Qi6LSAeNJeX3tp8EFcNduJK5VFs5mRY";
char xivelyKey[] = "0pV2Rl18sA1KHwG6lpP75mjbTUzdAYG5LI0HBuGDP14wrOj8";

// Define the strings for our datastream IDs
char sensorId[] = "Humidity";
char sensor2Id[] = "Temperature";
char sensor3Id[] = "Temperature_accurate";
char sensor4Id[] = "Light";
//char sensor5Id[] = "Rain";
char sensor5Id[] = "Voltage";
XivelyDatastream datastreams[] = {
  XivelyDatastream(sensorId, strlen(sensorId), DATASTREAM_FLOAT),
    XivelyDatastream(sensor2Id, strlen(sensor2Id), DATASTREAM_FLOAT),
      XivelyDatastream(sensor3Id, strlen(sensor3Id), DATASTREAM_FLOAT),
        XivelyDatastream(sensor4Id, strlen(sensor4Id), DATASTREAM_FLOAT),
           XivelyDatastream(sensor5Id, strlen(sensor5Id), DATASTREAM_FLOAT),
};
// Finally, wrap the datastreams into a feed
XivelyFeed feed(1292204656, datastreams, 5 );

EthernetClient client;
XivelyClient xivelyclient(client);

String message;
//String p, i;
char *tmp;
int i = 0;
float Data[5];


void setup() {
  // put your setup code here, to run once:
  //Serial.begin(9600);
  
  Serial.println("Starting single datastream upload to Xively...");
  Serial.println();

  Ethernet.begin(mac,ip,dnsIP,gateway,subnet);
  
  
  Mirf.cePin = 5;
  Mirf.csnPin = 6;
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"serv1");
  Mirf.payload = 1;
  Mirf.config();
}

void loop() {
  
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

       Serial.print(Data[0]);
       Serial.print("\t");
       Serial.print(Data[1]);
       Serial.print("\t");
       Serial.print(Data[2]);
       Serial.print("\t");
       Serial.print(Data[3]);
       Serial.println();
       Serial.print(Data[4]);
       Serial.println();
       message = "";
       //====================================================== XIVELY ==============================
  datastreams[0].setFloat(Data[0]);
  Serial.print("Humidity: ");
  Serial.println(datastreams[0].getFloat());
  Serial.println("Uploading it to Xively");
  
  datastreams[1].setFloat(Data[1]);
  Serial.print("Temperature: ");
  Serial.println(datastreams[1].getFloat());
  Serial.println("Uploading it to Xively");
   
  datastreams[2].setFloat(Data[2]);
  Serial.print("Temperature_accurate: ");
  Serial.println(datastreams[2].getFloat());
  Serial.println("Uploading it to Xively");
  
  datastreams[3].setFloat(Data[3]);
  Serial.print("Light: ");
  Serial.println(datastreams[3].getFloat());
  Serial.println("Uploading it to Xively");
    
  datastreams[4].setFloat(Data[4]);
  Serial.print("Voltage: ");
  Serial.println(datastreams[4].getFloat());
  Serial.println("Uploading it to Xively");
  

  int ret = xivelyclient.put(feed, xivelyKey);

     }
  }

  

}
