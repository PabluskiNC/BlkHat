// Adafruit_NeoMatrix example for single NeoPixel Shield.
// Scrolls 'Black Hat' across the matrix in a portrait (vertical) orientation.
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Wire.h>
#include <SPI.h>

#ifndef PSTR
 #define PSTR // Make Arduino Due happy
#endif

#define CR           13
#define BEL          7
#define BS           8
#define CAN          24

#define PIN 11

// Color definitions
#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0 
#define WHITE    0xFFFF

// MATRIX DECLARATION:
// Parameter 1 = width of NeoPixel matrix
// Parameter 2 = height of matrix
// Parameter 3 = pin number (most are valid)
// Parameter 4 = matrix layout flags, add together as needed:
//   NEO_MATRIX_TOP, NEO_MATRIX_BOTTOM, NEO_MATRIX_LEFT, NEO_MATRIX_RIGHT:
//     Position of the FIRST LED in the matrix; pick two, e.g.
//     NEO_MATRIX_TOP + NEO_MATRIX_LEFT for the top-left corner.
//   NEO_MATRIX_ROWS, NEO_MATRIX_COLUMNS: LEDs are arranged in horizontal
//     rows or in vertical columns, respectively; pick one or the other.
//   NEO_MATRIX_PROGRESSIVE, NEO_MATRIX_ZIGZAG: all rows/columns proceed
//     in the same order, or alternate lines reverse direction; pick one.
//   See example below for these values in action.
// Parameter 5 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)


// Example for NeoPixel Shield.  In this application we'd like to use it
// as a 5x8 tall matrix, with the USB port positioned at the top of the
// Arduino.  When held that way, the first pixel is at the top right, and
// lines are arranged in columns, progressive order.  The shield uses
// 800 KHz (v2) pixels that expect GRB color data.

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(32, 8, PIN,
  NEO_MATRIX_BOTTOM    + NEO_MATRIX_RIGHT +
  NEO_MATRIX_COLUMNS   + NEO_MATRIX_ZIGZAG,
  NEO_GRB           + NEO_KHZ800);

const uint16_t colors[] = {
  matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255) };

String inputString;         // a string to hold incoming data
String showString;
  char inChar;

boolean stringComplete = false;  // whether the string is complete

SoftwareSerial mySerial(2, 3); // RX, TX

void mySerialEvent();
void candyCane(uint8_t wait);
void theaterChaseRainbow(uint8_t wait);
uint32_t Wheel(byte WheelPos);
void BouncingBalls(int BallCount);

void setup() {
  Serial.begin(57600);
  mySerial.begin(57600);
  //attachInterrupt (digitalPinToInterrupt (2), mySerialEvent, CHANGE);  // Use pin 2 for the bluetooth 

  inputString.reserve(50);
  showString.reserve(50);
  inputString="Black Hat";

  stringComplete=true;
  
  matrix.begin();
  //matrix.setFont(&FreeSans9pt7b);
  matrix.setTextSize(0);

  matrix.setTextWrap(false);
  matrix.setBrightness(20);
  matrix.setTextColor(colors[0]);
}

int x    = matrix.width();
int pass = 0;
int x2   = x;
int ledSpeed = 50;
int ledBrightness = 20;

void loop() {

  mySerialEvent();

  if( stringComplete ) {
     Serial.print("Substring[");
     Serial.print(inputString.substring(0,2));
     Serial.println("]");
     if( inputString.substring(0,2).equalsIgnoreCase("M:") ) { 
        inputString.remove(0,2);
        Serial.print("Mode[");
        Serial.print(inputString.toInt());     
        Serial.println("]");

        int ledMode = inputString.toInt();
        switch( ledMode ) {
           case 1:  
              theaterChaseRainbow(ledSpeed);
              break;
           case 2:
              matrix.setBrightness(20);
              candyCane(ledSpeed);
              // put back the original brightness
              matrix.setBrightness(ledBrightness);
              break;
           case 3:
              BouncingBalls(5);
              break;
        }
        matrix.setTextColor(colors[0]);
        matrix.show();
        inputString="";
        stringComplete=false;
     }

     if( inputString.substring(0,2).equalsIgnoreCase("S:") ) { 
         inputString.remove(0,2);
         Serial.print("ledSpeed[");
         Serial.print(inputString.toInt());     
         Serial.println("]");
         int ledSpeedSel = inputString.toInt();
         Serial.print("ledSpeedSel[");
         Serial.print(ledSpeedSel);
         Serial.println("]");
         switch( ledSpeedSel ) {
            case 1: 
               ledSpeed = 100; 
               break;
            case 2:
               ledSpeed = 80;
               break;
            case 3:
               ledSpeed = 60;
               break;
            case 4:
               ledSpeed = 40;
               break;
            case 5:
               ledSpeed = 20;
               break;
            case 6:
               ledSpeed = 10;
               break;
            case 7:
               ledSpeed = 0;
               break;
         }
         Serial.print("Speed: ");
         Serial.println(ledSpeed);
         inputString="";
         stringComplete=false;

    }
    if ( inputString.substring(0,2).equalsIgnoreCase("B:") ) {
         inputString.remove(0,2);
         switch( inputString.toInt() ) {
            case 1:
               ledBrightness = 20;
               break;
            case 2: 
               ledBrightness = 80;
               break;
            case 3: 
               ledBrightness = 110;
               break;
            case 4: 
               ledBrightness = 175;
               break;
            case 5: 
               ledBrightness = 255;
               break;
         }
         Serial.print("Brightness: ");
         Serial.println(ledBrightness);
         matrix.begin();
         matrix.setBrightness(ledBrightness);
         inputString="";
         stringComplete=false;

      }
      if( stringComplete ) {    
        showString=inputString;
        Serial.print("Message to Show:[");
        Serial.print(showString);
        Serial.println("]");
        x = matrix.width();;
        x2 = x + (6 * showString.length());  // old formula
        //int16_t  nx, ny;
        //uint16_t nw, nh;
        // Get the string length in terms of pixels
        //matrix.getTextBounds(showString.c_str(),0,0,&nx,&ny,&nw,&nh);
        //x2=nw;
        
        inputString="";
        stringComplete=false;
     } 
  }
  
  matrix.fillScreen(0);
  matrix.setCursor(x, 0);

  matrix.print(showString);

  if(--x < -x2) {
    x = matrix.width();
    if(++pass >= 3) pass = 0;
  }
  
  matrix.setTextColor(colors[pass]);
  matrix.show();
  
  delay(ledSpeed);
}

/*---------------------------------------------------------------------------*/
// FUNCTIONS
/*---------------------------------------------------------------------------*/
void mySerialEvent() {
  while (mySerial.available()) {
    // get the new byte:
    char inChar = (char)mySerial.read();
    Serial.print(inChar);
    mySerial.print(inChar);
    // add it to the inputString:
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
        stringComplete = true;
    } else {
        inputString += inChar;
    }
  }
}

// red & white candy candy cane 
void candyCane(uint8_t wait) {
  matrix.fillScreen(0);  // blank screen
  matrix.show();
  // drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
  for(int i=0;i<10;i++) {
     for (int j=0;j<6;j++) {
        for(int ccx=0;ccx<43;ccx+=6) {
           matrix.fillRect(ccx-j  ,0,3,8,WHITE); //matrix.Color(127,0,0));
           matrix.fillRect(ccx-j+3,0,3,8,RED);
        }
        matrix.show();
        delay(wait);
     }
  }
  delay(500);
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (int i=0; i < matrix.numPixels(); i=i+3) {
        matrix.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      matrix.show();

      delay(wait);

      for (int i=0; i < matrix.numPixels(); i=i+3) {
        matrix.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return matrix.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return matrix.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return matrix.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void BouncingBalls(int BallCount) {
  float Gravity = -9.81;
  int StartHeight = 0;
 
  float Height[BallCount];
  float ImpactVelocityStart = sqrt( -2 * Gravity * StartHeight );
  float ImpactVelocity[BallCount];
  float TimeSinceLastBounce[BallCount];
  int   YPosition[BallCount];
  long  ClockTimeSinceLastBounce[BallCount];
  float Dampening[BallCount];
  int   num_leds = matrix.numPixels();
  int   ball_loops;
 
  for (int i = 0 ; i < BallCount ; i++) {  
    ClockTimeSinceLastBounce[i] = millis();
    Height[i] = StartHeight;
    //Height[i] = int(random(matrix.height()));
    //YPosition[i] = int(random(matrix.width()));
    YPosition[i] = ((matrix.width() - BallCount ) /2 ) + (i *2);
    ImpactVelocity[i] = ImpactVelocityStart;
    TimeSinceLastBounce[i] = 0;
    Dampening[i] = 0.90 - float(i)/pow(BallCount,2);
    //Serial.print("YPos:");
    //Serial.print(YPosition[i]);
  }

  ball_loops = 1000;
  while (ball_loops > 0) {
    ball_loops -= 1;
    for (int i = 0 ; i < BallCount ; i++) {
      TimeSinceLastBounce[i] =  millis() - ClockTimeSinceLastBounce[i];
      Height[i] = int(7 * (.5 * Gravity * pow( TimeSinceLastBounce[i]/1000 , 2.0 ) + ImpactVelocity[i] * TimeSinceLastBounce[i]/1000)) ;
 
      if ( Height[i] < 0 ) {                      
        Height[i] = 0;
        ImpactVelocity[i] = Dampening[i] * ImpactVelocity[i];
        ClockTimeSinceLastBounce[i] = millis();
 
        if ( ImpactVelocity[i] < 0.01 ) {
          ImpactVelocity[i] = ImpactVelocityStart;
        }
      }
      // Position[i] = round( Height[i] * (num_leds - 1) / StartHeight);
    }
 
    for (int i = 0 ; i < BallCount ; i++) {
      //matrix.setPixelColor(Position[i],0,0,0xff);
      matrix.drawPixel(YPosition[i],7 - Height[i],RED);
      /*
      Serial.print("Ball(");
      Serial.print(Height[i]);
      Serial.print(",");
      Serial.print(YPosition[i]);
      Serial.print(")\n");
      */
    }
   
    matrix.show();
    matrix.fillScreen(0);  // blank screen
    // setAll(0,0,0);
  }
}
