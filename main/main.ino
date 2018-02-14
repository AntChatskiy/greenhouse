/* 
 * This file is the main part of the program
 */
 
#include <LiquidCrystal.h>
#include <iarduino_RTC.h>
#include <Wire.h>
#include <DHT.h>



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
iarduino_RTC time(RTC_DS1307);   

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
int water; // variable for water sensor 
int watering_duration; 
int watering_time; // time for watering the plant
int dryness = 0; // the minimum level of humidity for the plant
float temp; // an actual temperature
float humidity; // an actual humidity level in %
float light; // an actual brightness level
const byte iter =  202; // divider for comfortble converting voltage to contingent designations
byte n; // variable to contingent designation
const int daylight = 0; // minimum level of brighness
const int twilight = 0; // really bad illumination 
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


void LampControl()
{
  light = analogRead(phRes);
  n = light/iter;
  if (n < daylight)
  {
    digitalWrite(lamp, HIGH);
  }
  else
  {
    digitalWrite(lamp, LOW);
  }
}


void Humidification(int duration, int humidity)
{
  humidity = dht.readHumidity();
  if (humidity < dryness) 
  {
    do {
    digitalWrite(mMaker, HIGH);
    } while (humidity < dryness);
    digitalWrite(mMaker, LOW);
  }
}


void Watering(int when, int duration, int rtc)
{
  duration = watering_duration;
  when = watering_time;
  rtc = time.gettime("H:i");
  if (when == rtc)
  {
    digitalWrite(pump, HIGH);
    delay(duration);
    digitalWrite(pump, LOW);   
  }
}


void DisplayMain(int rtc, int humidity, int temp)
{
  rtc = time.gettime("H:i");
  light = analogRead(phRes);
  n = light/iter;
  temp = dht.readTemperature();
  humidity = dht.readHumidity();

  // print time
  lcd.print(rtc);

  // print level of brightness
  lcd.setCursor(6, 0);
  if (n > daylight)
  {
    lcd.print("Afternon");
  } else if (n < daylight && n > twilight) {
    lcd.print("Twilight");
  } else if (n < twilight) {
    lcd.print("Night");
  }
  
  // print humidity level
  lcd.setCursor(0, 1);
  lcd.print("f ");
  lcd.setCursor(3, 1);
  lcd.print(humidity);
  if (humid < 10) 
  {
    lcd.setCursor(5,1);  
  } else {
    lcd.setCursor(6,1); 
  }
  lcd.print("%");

  // print temperature in celcium
  lcd.setCursor(8, 1);
  lcd.print(temp);
  if (temp < 10 && temp > -10) {
    lcd.setCursor(10, 1);
  } else {
    lcd.setCursor(11, 1);
  }
  lcd.print("C*");
}


void setup() 
{
  Serial.begin(9600);

  // RTC part
  Wire.begin();
  time.begin();
  time.settime(0,0,0,27, 14, 18, 5);  // sec, min, hour, month, year, day of the week

  // Humidity and temperature sensor part
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



void loop()
{
  
}



