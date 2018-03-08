#include <DHT.h>
#include <LiquidCrystal.h>
#include <iarduino_RTC.h>
iarduino_RTC time(RTC_DS1307);

// Water sensor
#define waterSensor A1

// Photoresistor
#define phRes A3

// Diod
#define diod 3

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

// Humidity and temperature sensor
#define DHTPIN 14 
#define DHTTYPE 22
DHT dht(DHTPIN, DHTTYPE);


#define buttonPins 4
const byte btns[] = {4, 5, 6, 7};

short check_time;

int min_humid = 50;
int real_humid = 60;
boolean humidification_status = false; // false - arduino do not watering now, true - arduino watering now

byte watering_hours = 12;
byte watering_minutes = 30;
byte intensity = 0;
boolean watering_status = false; 

short led_time; 
boolean led_on = true;

boolean lamp_is_on = false;
boolean mMaker_is_on = false;
boolean pump_is_on = false;


void setup()
{
  Serial.begin(9600);

  // Keyboard initializing
  for (short int i = 0; i < buttonPins; i++)
  {
    pinMode(btns[i], INPUT_PULLUP);
  }

  pinMode(diod, OUTPUT);
  
  pinMode(mMaker, OUTPUT);
    
  pinMode(pump, OUTPUT);

  pinMode(phRes, INPUT);

  pinMode(waterSensor,INPUT);
  
  // RTC part
  time.begin();
  time.settime(0,51,21,27,10,18,2); // sec, min, hours, day of month, year, day of week

  // DHT part
  dht.begin();
}


void loop()
{
  switch(checkButton(btns[1]))
  {
    case 1: // alarm mode
      digitalWrite(diod, HIGH); // alarm led
       
      delay(300); // delay just for user's comfort
      Serial.println("ALARM MODE");
      Serial.println("LAMP - 1btn");
      Serial.println("mMAKER - 3btn");
      Serial.println("MOTOR - 4btn");

      while(true)
      {
        // alarm led blinking
        if (!led_on & millis() - led_time > 1000)
        {
          digitalWrite(diod, HIGH);
          led_on = true;
          led_time = millis();
        }
        
        if (led_on && millis() - led_time > 1000)
        {
         digitalWrite(diod, LOW);
         led_on = false;
         led_time = millis();
        }
    
        if (checkButton(btns[1]))
        {
          break; // turning alarm mode off
        }
    
        if (checkButton(btns[0]) && !lamp_is_on)
        {
          digitalWrite(lamp, HIGH);
          lamp_is_on = true;
        }
    
        else if(checkButton(btns[0]) && lamp_is_on)
        {
          digitalWrite(lamp, LOW);
          lamp_is_on = false;
        }
        
        if (checkButton(btns[2]) && !mMaker_is_on)
        {
          digitalWrite(mMaker, HIGH);
          mMaker_is_on = true;
        }
    
        else if (!checkButton(btns[2]) && mMaker_is_on)
        {
          digitalWrite(mMaker, LOW);
          mMaker_is_on = false;
        }
    
        if (checkButton(btns[3]) && !pump_is_on)
        {
          digitalWrite(pump, HIGH);
          pump_is_on = true;
        }
    
        else if(!checkButton(btns[3]) && pump_is_on)
        {
          digitalWrite(pump, LOW);
          pump_is_on = false;
        }
     }
           
    default: // usual mode
      if (millis() - check_time > 1000)
      {
        inf_print(min_humid, watering_hours, watering_minutes);
        humidification_status = humidification(min_humid, humidification_status, real_humid);
        watering_status = watering(watering_hours, watering_minutes, intensity, watering_status);
        lighting();
        checkWater();
        
        check_time = millis();
          
        if (checkButton(btns[0]))
        {
          Serial.println("Menu - btn1");
          Serial.println("Watering settings - btn3");
          Serial.println("Min humidity lvl - btn4");
          delay(500);
        
          while(true)
          {
            if (checkButton(btns[0]))
            {
              break;
            }
        
            if (checkButton(btns[2]))
            {
              watering_hours, watering_minutes, intensity = setSettings(watering_hours, watering_minutes, intensity);
              Serial.print("Watering hours check: ");
              Serial.println(watering_hours);
              Serial.print("Watering minutes check: ");
              Serial.println(watering_minutes);
            }
        
            if (checkButton(btns[3]))
            {
              min_humid = setHumidity(min_humid);
            }
           }
        } 
      }      
  }
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


void inf_print(int min_humid, byte watering_hours, byte watering_minutes)
{
  /*
   * This function shows real-time condition of device
   */
  int real_humid = dht.readHumidity();
  
  Serial.print("Time: ");
  Serial.println(time.gettime("H:i:s"));

  Serial.print("Watering time: ");
  Serial.print(watering_hours);
  Serial.print(":");
  Serial.println(watering_minutes);
  
  Serial.print("Humidity level: ");
  Serial.print(real_humid);
  Serial.println("%");
  
  Serial.print("Min humidity level: ");
  Serial.print(min_humid);
  Serial.println("%");
}


void lighting()
{
  /*
   * This function illuminates the plant
   */
  int value = analogRead(phRes);
  if (time.Hours > 8 && time.Hours < 22 && value == 0)
  {
    Serial.println("LAMP HIGH");
    digitalWrite(lamp, HIGH);
  }
  
  else
  {
    Serial.println("LAMP LOW");
    digitalWrite(lamp, LOW);
  }
}

void checkWater()
{
  /*
   * This function checks water and if there's no water:
   * light the diod
   */
  int water = analogRead(A1);
  byte iter = 202;
  byte n = water/iter;
  if (n<5)
  {
    digitalWrite(diod, HIGH);
  }
  
  else
  {
    digitalWrite(diod, LOW);
  }
}


boolean watering(byte watering_hours, byte watering_minutes, byte intensity, boolean watering_status)
{
  /*
   * This function waters the plant
   * and return boolean information:
   * has watering already started or not
   */
  if (!watering_status && watering_hours == time.Hours && watering_minutes == time.minutes)
  {
    Serial.println("Pump HIGH");
    //digitalWrite(pump, HIGH);
    delay(intensity*10);
    return true;    
  }

  else if (watering_status && watering_hours == time.Hours && watering_minutes == time.minutes)
  {
    return true;
    Serial.println("Pump LOW");
  }
  
  else
  {
    Serial.println("Pump LOW");
    //digitalWrite(pump, LOW);
    return false;
  }

}


boolean humidification(int min_humid, boolean humidification_status, int real_humid)
{
  /*
   * This function humidifythe plant if 
   * it's required, or else 
   * it just return false and do nothing.
   */
  // int real_humid = (dht.readHumidity()+0,5);
  int required_humid = min_humid;
  while (required_humid < 100 && required_humid != (min_humid+20))
  {
    required_humid += 1;
  }

  if (!humidification_status && real_humid <= min_humid) 
  {
    Serial.println("mMaker HIGH");
    //digitalWrite(mMaker, HIGH);
    return true;
  }
  
  else if (humidification_status && real_humid < required_humid)
  {
    Serial.println("mMaker HIGH");
    return true;
  }
  
  else if (!humidification_status && real_humid > min_humid)
  {
    Serial.println("mMaker LOW");
    return false;
  }
  
  else if (humidification_status && real_humid >= required_humid)
  {
    Serial.println("mMaker LOW");
    //digitalWrite(mMaker, LOW);
    return false;
  }
}


byte setSettings(byte watering_hours, byte watering_minutes, byte intensity)
{
  /*
   * This function sets the settings of watering
   */
  Serial.print("Set watering time, now it is: ");
  Serial.print(watering_hours);
  Serial.print(":");
  Serial.println(watering_minutes);

  // set hours
  Serial.println("Set hours: ");
  delay(200);
  while(true)
  {
    if (checkButton(btns[2]) && watering_hours < 24)
    {
      watering_hours += 1;
      Serial.print("Hours: ");
      Serial.println(watering_hours);
    }

    if (checkButton(btns[3]) && watering_hours > 0)
    {
      watering_hours -= 1;
      Serial.print("Hours: ");
      Serial.println(watering_hours);
    }

    else if (checkButton(btns[3]) && watering_minutes <= 0)
    {
      Serial.print("Hours: ");
      Serial.println(watering_hours);
    }

    if (checkButton(btns[0]))
    {
      Serial.print("You've set hours: ");
      Serial.println(watering_hours);
      break;
    }

    delay(70);
  }
  
  delay(200);
  // set minutes
  Serial.println("Set minutes: ");
  delay(200);
  while(true)
  {
    if (checkButton(btns[2]) && watering_minutes < 60)
    {
      watering_minutes += 1;
      Serial.print("Minutes: ");
      Serial.println(watering_minutes);
    }

    if (checkButton(btns[3]) && watering_minutes > 0)
    {
      watering_minutes -= 1;
      Serial.print("Minutes: ");
      Serial.println(watering_minutes);
    }

    else if(checkButton(btns[3]) && watering_minutes <= 0)
    {
      Serial.print("Minutes: ");
      Serial.println(watering_minutes);
    }
    
    if (checkButton(btns[0]))
    {
      Serial.print("You've set minutes: ");
      Serial.println(watering_minutes);
      break;
    }

    delay(70);
  }

  delay(200);
  // set intensity
  Serial.println("Set intensity: ");
  delay(200);
  while(true)  
  {
    if (checkButton(btns[2]))
    {
      intensity += 1;
      Serial.print("Intensity: ");
      Serial.print(intensity);
      Serial.println(" sec");
    }

    if (checkButton(btns[3]) && intensity > 0)
    {
      intensity -= 1;
      Serial.print("Intensity: ");
      Serial.print(intensity);
      Serial.println(" sec");
    }

    else if (checkButton(btns[3]) && intensity <= 0)
    {
      Serial.print("Intensity: ");
      Serial.print(intensity);
      Serial.println(" sec");
    }
    
    if (checkButton(btns[0]))
    {
      Serial.print("You've set intensity: ");
      Serial.println(intensity);
      break;
    }    
  }

  return watering_hours, watering_minutes, intensity;
}


int setHumidity(int mid_humid)
{
  /* 
   * This function sets minimum humidity 
   * level   
   */

  Serial.println("Set minimum humidity level you want");

  while(true)
  {

    if (checkButton(btns[2]))
    {
        min_humid = (min_humid+1);
        Serial.print("Humidity level: ");
        Serial.print(min_humid);
        Serial.println("%");
    }

    if (checkButton(btns[3]))
    {
        min_humid = (min_humid-1);
        Serial.print("Humidity level: ");
        Serial.print(min_humid);
        Serial.println("%");
    }

    if (checkButton(btns[0]))
    {
        Serial.print("You've set min humidity level: ");
        Serial.print(min_humid);
        Serial.println("%");
        break;
    }

    delay(70);
  }

  return min_humid;
}


