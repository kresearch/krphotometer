#include <LiquidCrystal.h>   //use LCD library
// 30oct16 : macbook dev platform
// wirewrap build - UNO
// debugged tsl257 voltages
// moved detector to a3 to open i2c pins a4 a5
// code cleanup
// blue led wired with 1k resistor
// Revision: 1.2.0 - the krphotometer website rev
// Rev 1.2.1  14nov16 baseline and upload - roll rev on workdate of upload
//                   #Samples returned to 10 from 5
// Rev 1.2.2  15nov16 output the Rb value - 
//            corrected R for Green/Blue absorbtion ratio
//            RN calculation using default calibration
//            pH calculation from eq 1-8 using estimated default RN function

#define ProgName "krphotometer"
#define devSystem "mac"
#define Version "wirewrap"
#define Upload "15nov16"
#define Revision "2.1.2"

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
//#define Samples 5
#define Samples 10

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
lcd.clear();
// MiniSpec B.Y
lcd.print(ProgName);  //sample code signature - not KRDisplay Mini Spectrophotometer
lcd.print(" ");
lcd.print(devSystem);
lcd.setCursor(0, 1) ;
lcd.print(Upload);
lcd.print(" ");
lcd.print(Revision);

delay(1000); //Delay1000ms
lcd.setCursor(0,1);

// serial debug output
Serial.print(ProgName);
Serial.print(" ");
Serial.print(devSystem);
Serial.print(" ");
Serial.print(Version);
Serial.print("\nUpload date: ");
Serial.print(Upload);
Serial.print("\nVersion: ");
Serial.print(Version);
Serial.print("\nRevision: ");
Serial.print(Revision);
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
  lcd.print("BlankGrn(");
  lcd.print(x1a);

  lcd.print(")");
  lcd.setCursor(0, 1) ;
  lcd.print("BlankBlu(");
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
  x2a/= Samples; // green
 
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
  y2a /= Samples; // blue
  digitalWrite(BLUE,LOW);

  A_1 = log((float)x1a/(float)x2a)/(log(10));//calculate the absorbance  green
  A_2 = log((float)y1a/(float)y2a)/(log(10));//calculate the absorbance  blue
  R=(float)A_1/(float)A_2;
  // LOOK - correction - verified that R is correct  --- should be Green / Blue absorbtion 
  
  // EQ 8  - linear estimate of RN from RB (R here)
  double RN = 1.1892 * R - 0.3079;  // RN correction default from EQ 8 B Yang
  
  double pH, equConstant,R, e1,e2,e3, e23;
  double  T, S;  // temp and salinity
    // set default values for S and T
    // for salinity S values from 20 to 40
    // for temperatures T from 278.15 K to 308.15 K
   S =  30;
   T =  303;
   // eq 5
   e1 = -0.007762 + (4.5174 / pow(10.0,5.0)) * T;
   e23 = -0.020813 + (2.60262 / pow (10.0,4.0))*T + (1.0436 / pow (10.0,4.0)) * (S-35) ;

   // eq 7   equilibrium constant and components a,b,c,d  equConstant
   double a,b,c,d;

    a = -246.64209 + 0.315971 * S + (2.8855/pow (10.0,4.0)) * S * S ;
    b= 7229.23864 - 7.098137* S - 0.057034 * S * S;
    c = 44.493382-0.052711 * S;
    d = 0.0781344;
    equConstant = a + b / T + c * log(T) - d*T;  // log is the natural log ln

    pH = equConstant +  log10 (( RN - e1)/(1-RN*e23) );  // assume the COMMON LOG
  
  lcd.clear();
  lcd.print("AbsG ");
  lcd.print(A_1,3);  // green absb
  lcd.print("(");
  lcd.print(x2a);
  lcd.print(")");
  lcd.setCursor(0, 1) ;
  lcd.print("AbsB ");
  lcd.print(A_2,3);  // blue absb
  lcd.print("(");
  lcd.print(y2a);
  lcd.print(")");
  delay (2000);
  lcd.clear();
  lcd.print("RB ");  // Absb ration  of green/blue
  lcd.print(R); 
  lcd.setCursor(0, 1) ;
  lcd.print("RN ");
  lcd.print(RN);
  delay (2000);      // provide some analysis
  lcd.clear();
  lcd.print("RN ");
  lcd.print(RN);
  lcd.print("/");
  lcd.print(R);
  lcd.setCursor(0,1);
  lcd.print("PH ");
  lcd.print(pH);
 
  


  
  Serial.print("\n x1a ");
  Serial.print(x1a);
  Serial.print("\n x2a ");
  Serial.print(x2a);
  Serial.print("\n y1a ");
  Serial.print(y1a);
  Serial.print("\n y2a ");
  Serial.print(y2a);
  Serial.print("\n");
  Serial.print("Rb ");
  Serial.print(R);
    
  
}
}
