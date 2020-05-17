#include <FastLED.h>
#include "ledFunc.h"
#include "serialFunc.h"
#include "ledEffects.h"

//My defines

int previousMillis = 0;

#define interval 16  //62.5 fps refresh

bool ledstate = true;
bool adalightEN = true;
bool welcomemsg = false;

byte curMode = 1;   //defaults to adalight
byte brightness = 255;
byte ledR = 255;
byte ledG = 255;
byte ledB = 255;

void setup() 
{
  ledSetup();
  
  Serial1.begin(115200);
}

void loop() 
{
  recvWithStartEndMarkers();
  if (newData == true) 
  {
    strcpy(tempChars, receivedChars);
    parseData();
    
    //serialDebug();  //uncomment to enable serial input debug
    
    if(strcmp (messageRecv,"state") == 0)
    { 
      Serial1.println("<ack, 1, 2, 3>");
      adalightState(integer01Recv);
    }
  
    if(strcmp (messageRecv,"mode") == 0)
    {
      Serial1.println("<ack, 1, 2, 3>");
      adalightMode(integer01Recv);
    }
    
    if(strcmp (messageRecv,"brightness") == 0)
    {
      Serial1.println("<ack, 1, 2, 3>");
      adalightBrightness(integer01Recv);
    }
    
    if(strcmp (messageRecv,"ledRGB") == 0)
    {
      Serial1.println("<ack, 1, 2, 3>");
      adalightRGB(integer01Recv,  integer02Recv,  integer03Recv);
    }

    if(strcmp (messageRecv,"welcomemsg") == 0)
    {
      Serial1.println("<ack, 1, 2, 3>");
      welcomemsg = true;
    }
  
    newData = false;  //finishes serial data parsing
  }

  if (millis() - previousMillis > interval)  //loop program
  {
    previousMillis = millis();
    if(ledstate == true && adalightEN == false && welcomemsg == false)
    {
      switch(curMode)
      {
        case 2: //Custom mode
          LEDS.showColor(CRGB(ledR, ledG, ledB));
        break;

        case 3: //Mood lamp mode
        {
          FastLED.setBrightness(255);
          moodLamp();
        }
        break;
      }
    }
    else if(ledstate == false)    //turn off LEDs
    {
      LEDS.showColor(CRGB(0, 0, 0));
    }
  }

  if(adalightEN == true && ledstate == true && welcomemsg == false)  //Turn on adalight when needed and only when ledstate is on
  {
    adalight();
  }

  if(welcomemsg == true)
  {
    welcomemsgAni();
  }
}

// Comms
void adalightState(int state)
{
  if(state == 1)
  {
    ledstate = true;
    if(adalightEN == false && curMode == 2)
    {
      LEDS.showColor(CRGB(ledR, ledG, ledB));
    }
  }
  else if(state == 0)
  {
    ledstate = false;
  }
}

void adalightBrightness(unsigned int brightnesscur)
{
  brightness = brightnesscur;
  if(adalightEN == false  && curMode == 2) //only updates leds if adalight is not running and mode is in custom
  {
    FastLED.setBrightness(brightness);
    LEDS.showColor(CRGB(ledR, ledG, ledB));
  }
}

void adalightMode(int mode)
{
  if(mode != 1)
  {
    adalightEN = false;
  }
  else if(mode == 1)
  {
    FastLED.setBrightness(255);
    adalightEN = true;
  }
  
  switch(mode)  //one time run
  {
    case 2: //update LEDs
      FastLED.setBrightness(brightness);
      LEDS.showColor(CRGB(ledR, ledG, ledB));
    break;
  }
  curMode = mode;
}

void adalightRGB(int R, int G, int B)
{
  ledR = R;
  ledG = G;
  ledB = B;
}

void welcomemsgAni()
{
  FastLED.setBrightness(255);
  LEDS.showColor(CRGB(0, 0, 0));
  meteorRain(0xb7,0x00,0xfe,10, 64, true, 20);
  //CylonBounce(0xb7, 0x00, 0xfe, 4, 10, 50);
  welcomemsg = false;
}
