/* 
 * This file is the main part of the program
 */

#include <LiquidCrystal.h>
#include <Wire.h>
#include "RTClib.h"
RTC_DS1307 RTC;
LiquidCrystal lcd(lcdRS, lcdE, lcd4, lcd5, lcd6, lcd7)

//--------------------------------------
#define VCC 5    //Supply voltage

// Keyboard
#define btn_line 3
#define btn1 4
#define btn2 5
#define btn3 6
#define btn4 7

// Pump relay
#define pump 0 

// Mist maker relay
#define mMaker 1

// Lamp relay
#define lamp 2

// Display
#define lcd7 8
#define lcd6 9
#define lcd5 10
#define lcd4 11
#define lcdE 12
#define lcdRS 13

// RTC
#define SDA A4
#define SCL A5 

// Water sensor
#define water_sensor A0

// Photoresistor
#define phRes A3

//--------------------------------------

// Variables
float RT, VR, ln, TX, T0, VRT;
int water;
int water_time;
byte iter =  202;
byte n;
//--------------------------------------

bool CheckWater()
{
  water = analogRead(water_sensor);
  n = water/iter;
  if n < 5:
    return 1;
   else:
    return 0;
}


void setup() 
{
  Serial.begin(9600);

  // RTC part
  Wire.begin();
    RTC.begin();
  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  

  // Keyboard initializing
  pinMode(btn_line, INPUT);
  pinMode(btn1, INPUT);
  pinMode(btn2, INPUT);
  pinMode(btn3, INPUT);
  pinMode(btn4, INPUT);

  // Pump initializing
  pinMode(pump, OUTPUT);

  // Mist maker initializing
  pinMode(mMaker, OUTPUT);

  // Lamp initializing
  pinMode(lamp, OUTPUT);
}


void loop()
{
  
}



