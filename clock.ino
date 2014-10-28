//#include <Time.h>  
#include <Wire.h>
#include <SPI.h>
#include <RTClib.h>
#include <RTC_DS3231.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"
#include <Adafruit_NeoPixel.h>

#define PIN 6

Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);

Adafruit_7segment matrix = Adafruit_7segment();


RTC_DS3231 RTC;

#define TIME_MSG_LEN  11   // time sync to PC is HEADER followed by Unix time_t as ten ASCII digits
#define TIME_HEADER  'T'   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message 

int last_hour = 0;
const int buttonPin4 = 4;
const int buttonPin5 = 5;
int buttonState4 = 0;
int buttonState5 = 0;

int timmar;
int minuter;

void setup()  {
  
  pinMode(buttonPin4, INPUT);
  pinMode(buttonPin5, INPUT);
  digitalWrite(buttonPin4, HIGH);
  digitalWrite(buttonPin5, HIGH);
 
  strip.begin();
  strip.setBrightness(10);
  strip.show(); // Initialize all pixels to 'off'
  
  Serial.begin(9600);
  
#ifndef __AVR_ATtiny85__
//Serial.println("7 Segment Backpack");
#endif
  matrix.begin(0x70);
  matrix.drawColon(true);
  matrix.setBrightness(0);
  
  // Instantiate the RTC
  Wire.begin();
  RTC.begin();
 
  // Check if the RTC is running.
  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running");
  }

  // This section grabs the current datetime and compares it to
  // the compilation time.  If necessary, the RTC is updated.
  DateTime now = RTC.now();
  DateTime compiled = DateTime(__DATE__, __TIME__);
  if (now.unixtime() < compiled.unixtime()) {
    Serial.println("RTC is older than compile time! Updating");
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
 
  Serial.println("Setup complete.");
}

void loop(){    

buttonState5 = digitalRead(buttonPin5);
if (buttonState5 == LOW) {
  
  delay(200);
  
     strip.setPixelColor(0, 0, 255, 0);
     strip.show();
     DateTime now = RTC.now(); 
     int hours = now.hour();
     int minut = now.minute();
     int sec = now.second();
     sekunder(hours, minut);
     Serial.println("klar");
     Serial.print(timmar);
     Serial.print(" timmar och ");
     Serial.print(minuter);
     Serial.println(" minuter");
     int hdiff = timmar-hours;
     int mdiff = minuter-minut;
     Serial.println("Diff");
     Serial.print(hdiff);
     Serial.print(" timmar och ");
     Serial.print(mdiff);
     Serial.print(" minuter och ");
     Serial.println(sec);
     Serial.println(" sekunder");
     countdown(hdiff, mdiff, sec);

}
else
{
  digitalClockDisplay();  
  delay(1000);
  //rainbowCycle(20);
}

}

//Functions
////////////////////


void sekunder(int x, int y){
  delay(1000);
  Serial.println("add hours");
  buttonState4 = HIGH;    
  while(buttonState4 == HIGH){
  //buttonState3 = digitalRead(buttonPin3);
  buttonState4 = digitalRead(buttonPin4);
  buttonState5 = digitalRead(buttonPin5);
    delay(100);
  /*if (buttonState3 == LOW) {
    x += 1;
     if(x==24){x = 0;}
     Serial.print("Hours:");
     Serial.println(x);
     matrix.writeDigitNum(0, x/10);
     matrix.writeDigitNum(1, x%10);
     matrix.writeDisplay();
      }*/
  if (buttonState5 == LOW) {
     y += 1;
     if(y==60){y=0; x += 1; if(x==24){x=0;}}
     Serial.print("Minutes:");
     Serial.println(y);
     matrix.writeDigitNum(0, x/10);
     matrix.writeDigitNum(1, x%10);
     matrix.writeDigitNum(3, y/10);
     matrix.writeDigitNum(4, y%10);
     matrix.writeDisplay();
      }

  
  }
  timmar = x;
  minuter = y;
}


void countdown(int h,int m, int s)
{
  buttonState5 = HIGH;
  int total_time = h*60*60 + m*60 + s;
  int curr_time = total_time;
  int color = 0;
  colorWipe(strip.Color(0, 255, 0), 50); // Red
  
  while(m + s + h > 0 && buttonState5 == HIGH)
  {
     
     delay (1000);
     buttonState5 = digitalRead(buttonPin5);
      if(s==0&&m!=0)
        {
          m=m--;
          s=60;}
      if(s==0&&m==0&&h!=0){
        m=59;
        s=60;
        h=h--;
      }
      s=s--;
      curr_time = curr_time--;

    Serial.print("Hour: ");
    Serial.print(h);
    Serial.print(" Minutes: ");
    Serial.print(m);
    Serial.print(" Seconds: ");
    Serial.println(s);
    
    if(color == 0 && curr_time < (total_time/3)*2 ){
          colorWipe(strip.Color(0, 0, 255), 50); // Blue
          color = 1;
    }
    if(color == 1 && curr_time < total_time/3){
          colorWipe(strip.Color(255, 0, 0), 50); // Blue
          color = 2;
    }
   
    matrix.writeDigitNum(0, h/10);
    matrix.writeDigitNum(1, h%10);
    matrix.writeDigitNum(3, m/10);
    matrix.writeDigitNum(4, m%10);
    matrix.writeDisplay();
    
  }
  
  colorWipe(strip.Color(0, 0, 0), 50); // Blue
}


void digitalClockDisplay(){
  // digital clock display of the time
  DateTime now = RTC.now();
  matrix.writeDigitNum(0, now.hour()/10);
  matrix.writeDigitNum(1, now.hour()%10);
  matrix.writeDigitNum(3, now.minute()/10);
  matrix.writeDigitNum(4, now.minute()%10);
  matrix.writeDisplay();
  delay(500);
  Serial.print(now.hour());
  printDigits(now.minute());
  printDigits(now.second());
  Serial.println();
 /*if(now.hour() != last_hour){
   if(now.hour() == 07){
     colorWipe(strip.Color(0, 255, 0), 50); // Green
   }
   
   last_hour = now.hour();
 }*/ 
 
}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}


