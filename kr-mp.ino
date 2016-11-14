#include <LiquidCrystal.h>   //use LCD library
// 30oct16 : macbook dev platform
// wirewrap build - UNO
// debugged tsl257 voltages
// moved detector to a3 to open i2c pins a4 a5
// added 1k resistor to blue led to bring brightness into range 

#define ProgName "kr-mp"
#define devSystem "(mac)"
#define Version "mac wirewrap"
#definee Revision "1.2.0"

// if we swap blank and read buttons with green and blue  9 and 10 are pwm pins
// # define GREEN 7
// # define BLUE  6
#define TSL257 3
// # define BlankButton  10
// # define ReadButton 9
// -- moved pins so GREEN and BLUE are on PWM pins
// pwm did not work - and produced unstable, non linear readings- hardware solution works

#define GREEN 9
#define BLUE  8
#define BlankButton 7
#define ReadButton  6

//number of samples
#define Samples 5
//#define Samples 10

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);// LCD pin

int x1a=0;  // LOOK change to doouble
int y1a=0;
int x2a=0;
int y2a=0;
float A_1 = 0.000;//absorbance
float A_2 = 0.000;//absorbance
float R=0.000;
long vala = 0;  //"blank" button value
long valb = 0; //"sample" button value

void setup()
{
Serial.begin(9600);


lcd.begin(16, 2);  //Initialize LCD
// MiniSpec B.Y
lcd.print(ProgName);  //sample code signature - not KRDisplay Mini Spectrophotometer
lcd.print(" ");
lcd.print(devSystem);
lcd.setCursor(0, 1) ;
lcd.print(Version);
delay(1000); //Delay1000ms
Serial.print(ProgName);
Serial.print(" ");
Serial.print(devSystem);
Serial.print(" ");
Serial.print(Version);
Serial.print("\n");

pinMode(BlankButton,INPUT); //setup blank button
pinMode(ReadButton,INPUT);//setup sample button
pinMode(BLUE,OUTPUT);
pinMode(GREEN,OUTPUT);
digitalWrite(GREEN,HIGH);
delay(500);
digitalWrite(GREEN,LOW);
digitalWrite(BLUE,HIGH);
delay(500);
digitalWrite(BLUE,LOW);
}

void loop ()
{
if (digitalRead(BlankButton) == HIGH)
{
  lcd.clear(); //clear
  lcd.print("Blank Process");
  digitalWrite(GREEN,HIGH);//Green
  
  //analogWrite(GREEN,10);
  delay(1000);
     x1a = 0;
     int x = 0;
     for(int i=0; i<Samples; i++){
        x = analogRead(TSL257);
        x1a += x;
        //Serial.print(x);
        //Serial.print(" g\n");
        //delay(1000); // cool off the driver
     }

  digitalWrite(GREEN,LOW);
  x1a /= Samples ; //read the blank

  //Serial.print("\n");
  digitalWrite(BLUE,HIGH);//Blue
  delay(1000);
  y1a = 0;
  x=0;
  for(int i=0; i<Samples; i++){
       x =analogRead(TSL257);
       y1a += x;
       //Serial.print(x);
       //Serial.print(" b\n");
       //delay(40);
     }
  y1a /= Samples;
  digitalWrite(BLUE,LOW);
  //Serial.print("\n");

  lcd.clear(); //clear
  lcd.print("BlankG(");
  lcd.print(x1a);

  lcd.print(")");
  lcd.setCursor(0, 1) ;
  lcd.print("BlankB(");
  lcd.print(y1a);

  lcd.print(")");
}

if (digitalRead(ReadButton) == HIGH)
{
  lcd.clear(); //clear
  lcd.print("Read:green");

  digitalWrite(GREEN,HIGH);
  //analogWrite(GREEN,100); //LOOK
  delay(1000);
  x2a = 0;
   for(int i=0; i<Samples; i++){
                        x2a +=analogRead(TSL257);
                        //delay(40);
     }
  x2a/= Samples;
 
  digitalWrite(GREEN,LOW);
  
  lcd.clear();
  lcd.print("Read:blue");
  digitalWrite(BLUE,HIGH);
  //analogWrite(BLUE,1000);
  delay(1000);
  y2a = 0;
   for(int i=0; i<Samples; i++){
                        y2a +=analogRead(TSL257);
                        //delay(40);
     }
  y2a /= Samples;
  digitalWrite(BLUE,LOW);

  A_1 = log((float)x1a/(float)x2a)/(log(10));//calculate the absorbance
  A_2 = log((float)y1a/(float)y2a)/(log(10));//calculate the absorbance
  R=(float)A_2/(float)A_1;
  lcd.clear();
  lcd.print("AbsG ");
  lcd.print(A_1,3);
  lcd.print("(");
  lcd.print(x2a);
  lcd.print(")");
  lcd.setCursor(0, 1) ;
  lcd.print("AbsB ");
  lcd.print(A_2,3);
  lcd.print("(");
  lcd.print(y2a);
  lcd.print(")");
  
  Serial.print("\n x1a ");
  Serial.print(x1a);
  Serial.print("\n x2a ");
  Serial.print(x2a);
  Serial.print("\n y1a ");
  Serial.print(y1a);
  Serial.print("\n y2a ");
  Serial.print(y2a);
  Serial.print("\n");
}
}
