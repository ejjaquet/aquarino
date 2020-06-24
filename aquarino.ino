/**************
 * Aquarino - A basic aquarium controller for Arduino
 * developed by E.J. Jaquet
 **************/

// include Blynk and other credentials
#include "credentials.h"

/***************************************************
 *        Libraries used
 **************************************************/

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include "WiFiManager.h"

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#include <OneWire.h>
#include <DallasTemperature.h>

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
}
