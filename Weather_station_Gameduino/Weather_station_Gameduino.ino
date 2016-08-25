#include <EEPROM.h>
#include <SPI.h>
#include <GD2.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "floatToString.h"  //set to whatever is the location of floatToStrig
#include <SimpleTimer.h>
#include <Wire.h>
#include "RTClib.h"

SimpleTimer timer; // the timer object
RTC_DS1307 RTC;

# define PAINT_ALPHA() GD.BlendFunc(ONE, ONE_MINUS_SRC_ALPHA)
# define CLEAR_ALPHA() GD.BlendFunc(ZERO, ONE_MINUS_SRC_ALPHA)
#define ONE_WIRE_BUS 5

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// arrays to hold device address
DeviceAddress insideThermometer;

char buffer[25];
String message;
char *tmp;
int j = 0;
float Data[6];
int previous=0;
int counter=0;
int Pressure=1028;
float OutTemp=0, Humidity=0, DewPoint,HeatIndex, SysTemp, Voltage=0;

void setup()
{
  Serial.begin(9600);
  GD.begin();
  sensors.begin();
  Wire.begin();
  RTC.begin();
  if (! RTC.isrunning()) {
    while(1){
    GD.Clear();
    GD.Tag(100);
    GD.cmd_text(240, 136, 31, OPT_CENTER, "Error 1: RTC not running");
    GD.swap();
    GD.get_inputs();
  if(GD.inputs.tag==100){
    RTC.adjust(DateTime(2000, 1, 1, 12, 0, 0));
    RTC.adjust(DateTime(__DATE__, __TIME__));
    break;
    }
  }
  }
  sensors.getAddress(insideThermometer, 0);
  sensors.setResolution(insideThermometer, 12);
  sensors.requestTemperatures();
  timer.setInterval(30000, timeint);
  
  GD.cmd_loadimage(0, 0);
  GD.load("nightsky.jpg");
}

void loop()
{
  if(Serial.available()>0){
    char letter = Serial.read();
    
    if(letter != '~')
       { 
         message = String(message + letter);
       }
     else 
     {
        tmp = strtok(&message[0], ",");
        while (tmp) 
          {
           Data[j] = atof(tmp);
           tmp = strtok(NULL, ",");
           j++;
           }
           j=0;
           
           Humidity = Data[0];
           OutTemp  = Data[2];
           Voltage  = Data[4];
           
       
       //Serial.println();
       message = "";
     }
  }
    
    
    
    
    
  timer.run();
  float tempC = sensors.getTempC(insideThermometer);
  //draw_screen(millis()-previous,0,0,0,tempC);
  DewPoint = dewPointFast(OutTemp, Humidity);
  HeatIndex = heatIndex(OutTemp, Humidity);
  draw_screen(OutTemp,Humidity,Pressure,DewPoint,HeatIndex,Voltage,tempC);
  GD.get_inputs();
  if(GD.inputs.tag==1){

    while(GD.inputs.tag!=3){
        //GD.cmd_coldstart();
        GD.ClearColorRGB(0x103000);
        GD.Clear();
        GD.Tag(3);
        GD.cmd_text(450, 250, 29, OPT_CENTER, "Exit");
        GD.Tag(100);
        GD.cmd_text(240, 60, 30, OPT_CENTER, "CREDITS:");
        GD.cmd_text(240, 115, 29, OPT_CENTER, "Tsirogiannis Andreas");
        GD.cmd_text(240, 150, 29, OPT_CENTER, "30/5/2016");
        GD.cmd_text(240, 185, 29, OPT_CENTER, "Ver 1.0");
        GD.swap();
        GD.get_inputs();
        }
    }    
        
  if(GD.inputs.tag==2){

    while(GD.inputs.tag!=3){
        //GD.cmd_coldstart();
        GD.ClearColorRGB(0x50f030);
        GD.Clear();
        GD.Tag(3);
        GD.cmd_text(450, 250, 29, OPT_CENTER, "Exit");
        GD.swap();
        GD.get_inputs();
        }    
  }
  //previous=millis();
}

void draw_screen(float OutTemp, float Humidity,int Pressure, float DewPoint, float HeatIndex,float Voltage, float SysTemp)
{
DateTime now = RTC.now();  
GD.Clear(); // now alpha is all zeroes
GD.ColorMask(1,1,1,0); // draw tree, but leave alpha zero
GD.Begin(BITMAPS);
GD.Vertex2ii(0, 0);

  GD.ColorA(70);
  GD.Begin(RECTS);
  GD.Vertex2ii(8, 50);
  GD.Vertex2ii(200, 260);
  GD.ColorA(255);

GD.Tag(1);
GD.cmd_text(240, 25, 30, OPT_CENTER, "Solar Weather Station");
GD.Tag(100);
GD.cmd_text(95, 65, 28, OPT_CENTER, "Data:");
GD.cmd_text(15, 85, 27, 0, "OUT Temp:");
GD.cmd_text(15, 110, 27, 0, "Humidity:");
GD.cmd_text(15, 135, 27, 0, "Pressure:");
GD.cmd_text(15, 160, 27, 0, "Dew Point:");
GD.cmd_text(15, 185, 27, 0, "Heat Index:");
GD.cmd_text(15, 210, 27, 0, "Voltage:");
GD.cmd_text(15, 235, 27, 0, "Sys Temp:");

if(OutTemp<=0){
  GD.ColorRGB(0x002E94);
}
else if(OutTemp>0 && OutTemp<=38){
  GD.ColorRGB(0xffffff);
}
else if(OutTemp>38){
  GD.ColorRGB(0xfd4512);
}
degrees_c(160,85,OutTemp);


GD.ColorRGB(0xffffff);
GD.cmd_number(150, 110, 27, OPT_RIGHTX, Humidity);  GD.cmd_text(154, 110, 27, 0, "%");

GD.cmd_number(145, 135, 27, OPT_RIGHTX, Pressure);  GD.cmd_text(154, 135, 27, 0, "mBar");

if(DewPoint<=0){
  GD.ColorRGB(0x002E94);
}
else if(DewPoint>0 && DewPoint<=38){
  GD.ColorRGB(0xffffff);
}
else if(DewPoint>38){
  GD.ColorRGB(0xfd4512);
}
degrees_c(160,160,DewPoint);


if(HeatIndex<=0){
  GD.ColorRGB(0x002E94);
}
else if(HeatIndex>0 && HeatIndex<=38){
  GD.ColorRGB(0xffffff);
}
else if(HeatIndex>38){
  GD.ColorRGB(0xfd4512);
}
degrees_c(160,185,HeatIndex);

//GD.ColorRGB(0xffffff);
//GD.cmd_text(160, 210, 27, OPT_RIGHTX, "None");

if(Voltage<=11){
  GD.ColorRGB(0xfd4512);
  GD.cmd_text(45, 25, 30, OPT_CENTER, "!!!");
  GD.cmd_text(430, 25, 30, OPT_CENTER, "!!!");
}
else if(Voltage>11 && Voltage<=14){
  GD.ColorRGB(0xffffff);
}
else if(Voltage>14){
  GD.ColorRGB(0xfd4512);
  GD.cmd_text(45, 25, 30, OPT_CENTER, "!!!");
  GD.cmd_text(430, 25, 30, OPT_CENTER, "!!!");
}
GD.cmd_text(160, 210, 27, OPT_RIGHTX, (floatToString(buffer,Voltage,2)));  GD.cmd_text(168, 210, 27, 0, "V");

if(SysTemp<=20){
  GD.ColorRGB(0x002E94);
}
else if(SysTemp>20 && SysTemp<=35){
  GD.ColorRGB(0xffffff);
}
else if(SysTemp>35){
  GD.ColorRGB(0xfd4512);
}
degrees_c(160,235,SysTemp);

GD.ColorRGB(0xffffff);

GD.cmd_number(382, 200, 28, OPT_RIGHTX, now.day());
GD.cmd_text(385, 200, 28, 0, "/");
GD.cmd_number(403, 212, 28, OPT_CENTER, now.month());
GD.cmd_text(415, 200, 28, 0, "/");
GD.cmd_number(425, 200, 28, 0, now.year());

GD.cmd_number(391, 227, 28, OPT_RIGHTX, now.hour());
GD.cmd_text(395, 225, 28, 0, ":");
GD.cmd_number(413, 239, 28, OPT_CENTER, now.minute());
GD.cmd_text(427, 225, 28, 0, ":");
GD.cmd_number(435, 227, 28, 0, now.second());

GD.ColorMask(0,0,0,1);
GD.BlendFunc(ONE, ONE_MINUS_SRC_ALPHA);
GD.Begin(POINTS);
GD.PointSize(16 * 58); // outer circle
GD.Vertex2ii(410, 120);
GD.BlendFunc(ZERO, ONE_MINUS_SRC_ALPHA);
GD.PointSize(16 * 54); // inner circle
GD.Vertex2ii(410, 120);
GD.BlendFunc(ONE, ONE_MINUS_SRC_ALPHA);
GD.Tag(2);
GD.cmd_clock(410, 120, 55, OPT_NOBACK, now.hour(), now.minute(), now.second(), 0);

//  GD.Begin(RECTS);
//  GD.ColorRGB(0x6633ff);
//  GD.Vertex2ii(8, 60);
//  GD.Vertex2ii(180, 260);

GD.TagMask(0);
GD.ColorMask(1,1,1,0);
GD.BlendFunc(DST_ALPHA, ONE);
GD.ColorRGB(0xb0b0b0);
GD.Begin(RECTS); // Visit every pixel on the screen
GD.Vertex2ii(0,0);
GD.Vertex2ii(480,272);
GD.TagMask(1);

GD.swap();

}

static void degrees_c(int x, int y, float d)
{
  GD.cmd_text(x , y, 27, OPT_RIGHTX, (floatToString(buffer,d,2)));
  //GD.cmd_number(x, y, 27, OPT_RIGHTX | OPT_SIGNED , d);
  GD.cmd_text(x + 4, y, 26, 0, "o");
  GD.cmd_text(x + 8, y, 27, 0, " C");
}

void timeint(){
  sensors.requestTemperatures();
}

double dewPointFast(double celsius, double humidity)
{
 double a = 17.271;
 double b = 237.7;
 double temp = (a * celsius) / (b + celsius) + log(humidity*0.01);
 double Td = (b * temp) / (a - temp);
 return Td;
}

double heatIndex(double tempC, double humidity)
{
  double tempF = tempC * 1.8 + 32; 
  
  double c1 = -42.38, c2 = 2.049, c3 = 10.14, c4 = -0.2248, c5= -6.838e-3, c6=-5.482e-2, c7=1.228e-3, c8=8.528e-4, c9=-1.99e-6  ;
  double T = tempF;
  double R = humidity;

  double A = (( c5 * T) + c2) * T + c1;
  double B = ((c7 * T) + c4) * T + c3;
  double C = ((c9 * T) + c8) * T + c6;

  double rv = (C * R + B) * R + A;
  rv = (rv - 32) / 1.8;
  return rv;
}
