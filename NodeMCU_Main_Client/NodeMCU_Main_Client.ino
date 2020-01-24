#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "wifiConf.h"
#include "mqttfunc.h"
#include "OTAfunc.h"

#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

unsigned long previousMillis = 0, currentMillis;
unsigned int interval = 15000;

String ledR, ledG, ledB;

void setup() 
{
  Serial.begin(115200); // Serial port for STM32F103C8T6 communication
  //Serial1.begin(115200);  // Usual USB/Serial port. Uncomment to enable serial debugging
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  OTAfunc();
}

void callback(char* topic, byte* payload, unsigned int length) 
{
  String topicstr;
  String payloadstr;
  
  for (int i = 0; i < length; i++) 
  {
    payloadstr = String(payloadstr + (char)payload[i]);  //convert payload to string
  }
  
  for(int i = 0; i <= 50; i++)
  {
    topicstr = String(topicstr + (char)topic[i]);  //convert topic to string
  }

  Serial1.print("Message arrived: ");
  Serial1.print(topicstr);
  Serial1.print(" - ");
  Serial1.print(payloadstr);
  Serial1.println();
  
  if(topicstr=="/adalight/statecmd")
  { 
    if(payloadstr=="1") //On Adalight
    {
      client.publish("/adalight/state", "1"); //publish to topic
      Serial.print("<state, 1>");
    }
    else if(payloadstr=="0") //Off Adalight
    {
      client.publish("/adalight/state", "0"); //publish to topic 
      Serial.print("<state, 0>");
    } 
  }
  
  if(topicstr=="/adalight/mode")
  {
    Serial.print("<mode, " + payloadstr + ">");
  }

  if(topicstr=="/adalight/brightness")
  { 
    Serial.print("<brightness, " + payloadstr + ">");
  }
  
  if(topicstr=="/adalight/R")
  {
    ledR = payloadstr;
  }
  if(topicstr=="/adalight/G")
  {
    ledG = payloadstr;
  }
  if(topicstr=="/adalight/B")
  {
    ledB = payloadstr;
    Serial.print("<ledRGB, " + ledR + "," + ledG + "," + ledB + ">");
  }

  if(topicstr=="/adalight/welcomemessage")
  { 
    Serial.print("<welcomemsg, 55>");
  }

  if(topicstr == "/main_node/reboot") //exposes reboot function
  {
    ESP.restart();
  }
}

void loop() 
{
  currentMillis = millis();
  
  if (!client.connected()) 
  {
    reconnect();
  }
  client.loop();
  ArduinoOTA.handle();

  if (currentMillis - previousMillis >= interval) 
  {
    previousMillis = currentMillis;
    client.publish("/nodemcu/heartbeat", "Hi");
  }
  
  if(currentMillis> 4094967296)
  {
    ESP.restart();
  }
}
