#include <Arduino.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <NTPClient.h>
#include <time.h>
#include "LedMatrix.h"
#include "config.h"

#define NUMBER_OF_DEVICES 4
#define CS_PIN D0
LedMatrix ledMatrix = LedMatrix(NUMBER_OF_DEVICES, CS_PIN);
char incomingPacket[255];

WiFiUDP ntpUDP, udp;
NTPClient timeClient(ntpUDP, "ntp.nic.cz", 3600, 60000);

String getFullFormattedTime() {
   time_t rawtime = timeClient.getEpochTime();
   struct tm * ti;
   ti = localtime (&rawtime);

   uint16_t year = ti->tm_year + 1900;
   String yearStr = String(year);

   uint8_t month = ti->tm_mon + 1;
   String monthStr = month < 10 ? "0" + String(month) : String(month);

   uint8_t day = ti->tm_mday;
   String dayStr = day < 10 ? "0" + String(day) : String(day);

   uint8_t hours = ti->tm_hour;
   String hoursStr = hours < 10 ? "0" + String(hours) : String(hours);

   uint8_t minutes = ti->tm_min;
   String minuteStr = minutes < 10 ? "0" + String(minutes) : String(minutes);

   uint8_t seconds = ti->tm_sec;
   String secondStr = seconds < 10 ? "0" + String(seconds) : String(seconds);

   return yearStr + "-" + monthStr + "-" + dayStr + " " +
          hoursStr + ":" + minuteStr + ":" + secondStr;
}


int x = 0;
void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);

    pinMode(LED_BUILTIN, OUTPUT);
    ledMatrix.init();
    ledMatrix.setRotation(true);
    ledMatrix.setText("ahoj");

 WiFiManager wifiManager;
 wifiManager.autoConnect(WIFI_CRED);

 timeClient.begin();

 udp.begin(12345);
}

void loop() {
  ledMatrix.clear();
  ledMatrix.scrollTextLeft();
  //ledMatrix.oscillateText();
  ledMatrix.drawText();
  ledMatrix.commit();
  delay(50);
  x += 1;
  if (x % 400 == 0) {
     //ledMatrix.setNextText(timeClient.getFormattedTime());
     ledMatrix.setNextText(getFullFormattedTime());
  }

  timeClient.update();


  int packetSize = udp.parsePacket();
  if (packetSize)
  {
    Serial.printf("Received %d bytes from %s, port %d\n", packetSize, udp.remoteIP().toString().c_str(), udp.remotePort());
    int len = udp.read(incomingPacket, 255);
    if (len > 0)
    {
      incomingPacket[len] = 0;
    }
    Serial.printf("UDP packet contents: %s\n", incomingPacket);
    ledMatrix.setText(incomingPacket);
    x = 0;
  }

}
