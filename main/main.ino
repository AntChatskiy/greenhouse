/* 
 * This file is the main part of the program
 */

 //Values from datasheet
#define RT0 10000   // Оm
#define B 3977      // K
//--------------------------------------
#define VCC 5    //Supply voltage
#define R 10000  //R=10 КОm
#define Relay 4 

//переменные
float RT, VR, ln, TX, T0, VRT;


int ReadTemp()
{
  VRT = analogRead(A0);              //Reading the analog value of VRT
  VRT = (5.00 / 1023.00) * VRT;      //Convert to voltage
  VR = VCC - VRT;
  RT = VRT / (VR / R);               //RT resistance

  ln = log(RT / RT0);
  TX = (1 / ((ln / B) + (1 / T0))); //Temperature from thermistor

  TX = TX - 273.15;                 //Convert to celsius

  Serial.print("Temperature:");
  Serial.print("\t");
  Serial.print(TX);
  Serial.print("C\t\t");
  delay(500);
}

int TurnLightOn()
{
  digitalWrite(Relay, LOW);
}

int TurnLightOf()
{
  digitalWrite(Relay, HIGH);
}

bool CheckWater()
{
  return true;
}

void setup() {
  Serial.begin(9600);
  T0 = 25 + 273.15;                 //Temperature from Datasheet, converting from celsius to kelvin

  pinMode(2, OUTPUT); //the relay (lamp)
}

void loop(){
  
}



