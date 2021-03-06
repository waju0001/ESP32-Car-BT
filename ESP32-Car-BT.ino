/*
 * Program: ESP32-Car-BT.ino
 * 20/02/20 Tareq Mahwdawi / Juergen Walter
 * Hardware: eMalRob ESP32 + H-Bridge 8833 + OLED + 2 DC Motors + 1 Servo
 * Software: Android - Arduino bluetooth controller
 * 
 */


#include<arduino.h>
#include <ESP32MotorControl.h>
#include "BluetoothSerial.h" 
#include <ESP32Servo.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>

#define PIN 14                                          //LED Ausgangs-Pin
#define NUMPIXELS 90

//OLED
#define OLED_RESET 15                //  15 changed - not used nicht genutzt bei diesem Display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
//Adafruit_SSD1306 display(OLED_RESET); Treiber vor Version1.2
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Servo myservo; 
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800); //LED
BluetoothSerial SerialBT; 
#define AIN1 17
#define AIN2 16
#define BIN1 2
#define BIN2 4
#define Start 35
#define MotorEN 27
#define Encoder 25

int pos = 0;    // variable to store the servo position
// Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33 
int servoPin = 13;
int incoming=0;
int N=0;
int t=0;
int timeout=0;
int speedcontrol=0;
ESP32MotorControl MotorControl = ESP32MotorControl();
void setup() 
{
  SerialBT.begin("ESP32_CarControl_1"); 
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); 
  display.display();
  delay(2000);
   display.clearDisplay();

  // text display tests
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("eMalRob 20");
  display.setCursor(0,32);
  display.println("BT-Control");
   display.setCursor(48,48);
  display.println("1");
  display.display();
  pinMode(PIN,OUTPUT);
  pinMode(AIN1,OUTPUT);
  pinMode(AIN2,OUTPUT);
  pinMode(BIN1,OUTPUT);
  pinMode(BIN1,OUTPUT);
  pinMode(MotorEN,OUTPUT);
  pinMode(Encoder,INPUT);
  pinMode(Start,INPUT);
  pixels.begin(); //LED
  
  MotorControl.attachMotors(AIN1, AIN2, BIN1, BIN2);
  myservo.setPeriodHertz(50);    // standard 50 hz servo
  myservo.attach(servoPin, 1000, 2000); // attaches the servo on pin 18 to the servo object
  myservo.write(pos);  
  delay(15);
  for(int8_t i=0; i<=10; i++)
  {
      pixels.setPixelColor(i,0,0,0);
  }
  pixels.show();
}

void loop() 
{   
  float Status ;
  float s1;
  Status =analogRead(Start);
  s1=Status;
  if(s1==0)
  {
    for(int8_t i=0; i<=10; i++)
    {
        pixels.setPixelColor(i,0, 255,0);
    }
      pixels.show();
      N=1;
      Start_Anzeige();
      pos=170;
      myservo.write(pos);  
      delay(15);
     for (int i=0;i<10;i++)
     {

        digitalWrite(MotorEN,HIGH);
        MotorControl.motorForward(0, 100);
        MotorControl.motorForward(1, 0);
        //MotorControl.motorReverse(1, 50);
        delay(3000);
        MotorControl.motorForward(0, 100);
        MotorControl.motorForward(1, 100);
        delay(200);

     }

      digitalWrite(MotorEN,LOW);
      pos=0;
      myservo.write(pos); 
    for(int8_t i=0; i<=10; i++)
    {
        pixels.setPixelColor(i,255,0,0);
    }
      pixels.show();
     
    }
   
   if (SerialBT.available()) 
   {
      incoming = SerialBT.read();
      Serial.println(incoming);
      millis();
      if(incoming == 57)
       {
          if(t==1)
          {
            t=0;
            speedcontrol=50;
          }
          else
          {
            t=1;
            speedcontrol=90;
          }
        }
       
      if(incoming == 48)
       {
          Geradelicht();
          Vorwaerts_Anzeige();
          digitalWrite(MotorEN,HIGH);
          MotorControl.motorForward(0, speedcontrol);
          MotorControl.motorForward(1, speedcontrol);
          incoming = SerialBT.read();
          
       }
      if (incoming == 49)
       {
          Reverselicht();
          Rueckwaerts_Anzeige();
          digitalWrite(MotorEN,HIGH);
          MotorControl.motorReverse(0, speedcontrol);
          MotorControl.motorReverse(1, speedcontrol);
          incoming = SerialBT.read();
       }
       if(incoming == 50)
       {
          Blinkrechts();
          Rechts_Anzeige();
          digitalWrite(MotorEN,HIGH);
          MotorControl.motorForward(0,0);
          MotorControl.motorForward(1, speedcontrol);
          incoming = SerialBT.read();   
      }
      if(incoming == 51)
      {
          Blinklinks();
          Links_Anzeige();
          digitalWrite(MotorEN,HIGH);
          MotorControl.motorForward(0, speedcontrol);
          MotorControl.motorForward(1,0);
          incoming = SerialBT.read();   
      }
      if(incoming == 54)
      {
          Dreh_Links_Anzeige();
          Dreh_links_Licht();
          digitalWrite(MotorEN,HIGH);
          MotorControl.motorForward(0, speedcontrol);
          MotorControl.motorReverse(1,speedcontrol);
          incoming = SerialBT.read();   
      }
      if(incoming == 55)
      {
          Dreh_Rechts_Licht();
          Dreh_Rechts_Anzeige();
          digitalWrite(MotorEN,HIGH);
          MotorControl.motorForward(1, speedcontrol);
          MotorControl.motorReverse(0,speedcontrol);
          incoming = SerialBT.read();   
      }
      if(incoming == 53)
      {
         Stoplicht();
         Ende_Anzeige();
         digitalWrite(MotorEN,LOW);
         incoming = SerialBT.read();
      }
      if(incoming == 56)
      {
       
          if(N==1)
          {
              N=0;
              myservo.write(0);
          }
          else
          {
              N=1;
              myservo.write(170);
          }
      }
      if(incoming == 52)
      {
          if(N==1)
          {
               N=0;
               Ende_Anzeige();
               speedcontrol=0;
               for(int8_t i=0; i<=10; i++)
               {
                  pixels.setPixelColor(i,0,0,0);
               }
               pixels.show();
          }
          else
          {
              N=1;
              Start_Anzeige();
              speedcontrol=50;
              for(int8_t i=0; i<=10; i++)
              {
                  pixels.setPixelColor(i,255,240,245);
              }
              pixels.show();
          }
      } 
  }

pixels.show();
 
}
void Ende_Anzeige()
{
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("eMalRob 20");
    display.setCursor(32,32);
    display.println("Stop");
    display.display();
     
}
void Vorwaerts_Anzeige()
{
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("eMalRob 20");
    display.setCursor(0,32);
    display.println("Vorwaerts");
    display.display();
}

void Start_Anzeige()
{
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("eMalRob 20");
    display.setCursor(32,32);
    display.println("START");
    display.display(); 
}
void Rueckwaerts_Anzeige()
{
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("eMalRob 20");
    display.setCursor(32,32);
    display.println("Rueckwaerts");
    display.display();
}
void Rechts_Anzeige()
{
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("eMalRob 20");
    display.setCursor(0,32);
    display.println("nach Rechts");
    display.display();
}
void Links_Anzeige()
{
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("eMalRob 20");
    display.setCursor(0,32);
    display.println("nach Links");
    display.display();
}
void Dreh_Links_Anzeige()
{
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("eMalRob 20");
    display.setCursor(32,32);
    display.println("Drehlinks");
    display.display();
}
void Dreh_Rechts_Anzeige()
{
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("eMalRob 20");
    display.setCursor(0,32);
    display.println("Drehrechts");
    display.display();
}
void Blinkrechts()
{
    for(int8_t i=0; i<=4; i++)
    {

        pixels.setPixelColor(i,255, 165,0);

    }
    for(int8_t i=5; i<=10; i++)
    {

        pixels.setPixelColor(i,0, 100,0);

    }
    pixels.show();  
}

void Blinklinks()
{
    for(int8_t i=6; i<=10; i++)
    {
        pixels.setPixelColor(i,255, 165,0);
    }
    for(int8_t i=0; i<=5; i++)
    {
        pixels.setPixelColor(i,0, 100,0);
    }
    pixels.show();  
}
void Geradelicht()
{
    for(int8_t i=0; i<=10; i++)
    {
      pixels.setPixelColor(i,0,100,0);
    }
    pixels.show();
}

void Reverselicht()
{
    for(int8_t i=0; i<=10; i++)
    {
      pixels.setPixelColor(i,255, 165,0);
    }
    pixels.show();
}
void Stoplicht()
{
    for(int8_t i=0; i<=10; i++)
    {
      pixels.setPixelColor(i,255,0,0);
    }
    pixels.show();
}
void Dreh_links_Licht()
{
    for(int8_t i=0; i<=10; i++)
    {
      pixels.setPixelColor(i,0,0,255);
    }
    pixels.show();
}
void Dreh_Rechts_Licht()
{
    for(int8_t i=0; i<=10; i++)
    {
      pixels.setPixelColor(i,0,206,209);
    }
    pixels.show();
}
