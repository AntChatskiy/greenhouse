#include <TroykaDHT.h>
#include <iarduino_RTC.h>
#include <cstdlib>
iarduino_RTC time(RTC_DS1307);

// Water sensor
#define waterSensor A1

// Photoresistor
#define phRes A3

// Diod
#define diod 0

// Blower relay
#define blower 8

// Mist maker relay
#define mMaker 9

// Lamp relay
#define lamp 10

// Humidity and temperature sensor
#define DHTPIN 4
#define DHTTYPE 11
DHT dht(DHTPIN, DHTTYPE);

int water;

short check_time;

int min_humid = 50;
int real_humid = 60;
boolean humidification_status = false; // false - arduino do not watering now, true - arduino watering now

int min_light = 1;
int day_hours = 8;

void setup()
{
  Serial.begin(9600);
  
  pinMode(mMaker, OUTPUT);

  pinMode(phRes, INPUT);

  pinMode(waterSensor,INPUT);
  
  // RTC part
  time.begin();

  // DHT part
  dht.begin();
}


void loop()
{    
  if (millis() - check_time > 970)
  {
    pc_output();
    min_humid, day_hours = pc_input(min_humid, day_hours);
    check_time = millis();
  }
  
  humidification_status = humidification(min_humid, humidification_status);
  lighting(min_light);
  checkWater();         
}


boolean checkButton(byte btn)
{
  delay(10);
  boolean buttonIsUp = digitalRead(btn);
  
  if (!buttonIsUp)
  { 
    delay(10);
    buttonIsUp = digitalRead(btn);
    if (!buttonIsUp)
    {
        return true;
    }
  }
    
  else
  {
    return false;
  }
}



int pc_input(int min_humid, int day_hours)
{
  
   /*
   * This function sends to computer real-time condition of device
   */
  for (byte i = 0, i<2, i++)
  {    
    if(Serial.available() && Serial.available != "\n") 
    {
      if (i == 0)
      {
        min_humid = Serial.parseInt();
      }

      else if (i == 1)
      {
        day_hours = Serial.parseInt();
      }
    }
  }
  return min_humid, day_hours
}


void pc_output()
{
  /*
   * This function writes into COM-port
   * total 15 bytes
   * hours:minutes_real humidity_lightness_is water enough \n
   */
  dht.read();
  int real_humid = dht.getHumidity();

  Serial.print(time.gettime("H:i:s"));
  Serial.print(" ");

  Serial.print(real_humid);
  Serial.print(" ");
  
  Serial.print(analogRead(phRes)/51);
  Serial.print(" ");

  if (check_water() == true)
  {
    Serial.println("True");
  }

  else
  {
    Serial.println("False");  
  }
}

void lighting(int day_hours)
{
  /*
   * This function illuminates the plant
   */
  int value = analogRead(phRes);
  
  if (time.Hours > 6 && time.Hours < (6+day_hours) && value < 650)
  {
    digitalWrite(lamp, LOW);
  }
 
  else
  {
    digitalWrite(lamp, HIGH);
  }
}


boolean checkWater()
{
  /*
   * This function checks water and if there's no water:
   * light the diod
   */
  int water = analogRead(A1);
  byte iter = 202;
  byte n = water/iter;
  if (n<3)
  {
    return true;
  }
  
  else
  {
    return false;
  }
}


boolean humidification(int min_humid, boolean humidification_status)
{
  /*
   * This function humidifythe plant if 
   * it's required, or else 
   * it just return false and do nothing.
   */
  
  dht.read();
  int real_humid = dht.getHumidity();
  
  int required_humid = min_humid;
  while (required_humid < 100 || required_humid != (min_humid+20))
  {
    required_humid += 1;
  }

  if (!humidification_status && real_humid <= min_humid) 
  {
    digitalWrite(mMaker, LOW);
    digitalWrite(blower, LOW);
    return true;
  }
  
  else if (humidification_status && real_humid < required_humid)
  {
    return true;
  }
  
  else if (!humidification_status && real_humid > min_humid)
  {
    return false;
  }
  
  else if (humidification_status && real_humid >= required_humid)
  {
    digitalWrite(mMaker, HIGH);
    digitalWrite(blower, HIGH);
    return false;
  }
}


