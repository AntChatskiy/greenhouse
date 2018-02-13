/* 
 * This file is the main part of the program
 */
 
#include "DHT.h" 
#include <LiquidCrystal.h>
#include <Wire.h>
#include "RTClib.h"


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
LiquidCrystal lcd(lcdRS, lcdE, lcd4, lcd5, lcd6, lcd7);

// RTC
#define SDA A4
#define SCL A5 
RTC_DS1307 RTC;

// Water sensor
#define water_sensor A0

// Humidity and temperature sensor
#define DHTPIN 14
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Photoresistor
#define phRes A3

//--------------------------------------

// Variables
float RT, VR, ln, TX, T0, VRT;
int water;
int water_time;
int dryness = 0;
float temp;
float humid;
float light;
byte iter =  202;
byte n;
const int daytime = 0; 
//--------------------------------------


bool CheckWater()
{
  water = analogRead(water_sensor);
  n = water/iter;
  if (n < 5)
  {
    return true;
  }
   else
   {
    return false;
   }  
}


double Humidity()
{
  return humid = dht.readHumidity();
}


double Temperature()
{
  return temp = dht.readTemperature();
}


double Lightness()
{
  return light = analogRead(phRes);  
}


void LampControl()
{
  light = Lightness();
  n = light/iter;
  if (n < daytime)
  {
    digitalWrite(lamp, HIGH);
  }
  else
  {
    digitalWrite(lamp, LOW);
  }
}


void Humidification(duration)
{
  humid = Humidity()
  if (humid < dryness)
  {
    digitalWrite(mMaker, HIGH);
    delay(duration);
    digitalWrite(mMaker, LOW);
  }
}


void Watering(int when, int duration, int rtc)
{
  rtc = time.gettime("H:i");
  if (when == rtc)
  {
    digitalWrite(pump, HIGH);
    delay(duration);
    digitalWrite(pump, LOW);   
  }
}


void DisplayMain(int rtc)
{
  rtc = time.gettime("H:i");
  lcd.print(time.gettime("H:i"));
  lcd.setCursor(0,1);
  lcd.print("f ", humid = Humidity(), " %");
  lcd.setCursor(8,1);
  lcd.print(temp = Temperature(), " *C");
}


void setup() 
{
  Serial.begin(9600);

  // RTC part
  Wire.begin();
    RTC.begin();
  if (! RTC.isrunning()) 
  {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));

  // Humidity sensor part
  dht.begin();

  // Display part
  lcd.begin(16, 2);
  
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
}


void loop()
{
  
}



