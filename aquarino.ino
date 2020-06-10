/*
 * Aquarino - A basic aquarium controller for Arduino
 * developed by E.J. Jaquet
 * 
 *
 * PIN AANSLUITINGEN PH SENSOR
 *
 * Vcc  --> 5V
 * Data --> A0
 * GND  --> GND
 * GND --> GND
 *
 *
 * PIN AANSLUITINGEN 1-wire SENSOR
 *
 * GND  --> GND
 * Data --> D7
 * Vcc  --> 5V
 *
 *
 *
 * PIN AANSLUITINGEN LCD I2C
 * 
 * SDA is serial data
 * SCL is serial clock
 *
 * GND --> GND
 * VCC --> 5V
 * SDA --> D2
 * SCL --> D1
 * 
 *  
 */

//// Blynk (iot) ////

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <BlynkSimpleEsp8266.h>

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include "WiFiManager.h"          //https://github.com/tzapu/WiFiManager

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "thEHa5ITbUeO5xq7ITLAVyp4Ek3lLzcX";

BlynkTimer timer;

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
}

// This function sends Arduino's up time every second to Virtual Pin (5).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.
void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V5, millis() / 1000);
}

//// PH-SENSOR ////

static const int analogInPin = A0; 
int sensorValue = 0; 
unsigned long int avgValue;
float calibration = -0.50;               //change this value to calibrate
float b;
int buf[10],temp;

float phValue = 0.0;                    // variabele om ph waarde in op te slaan (float is getal met komma)

int intervalPh = 1000;                  // Tijd in milliseconden tussen het uitlezen van de ph sensor
unsigned long previousMillisPh = 0;     // Tijdstip van laatste uitlezing ph sensor

//// LCD SCHERM ////

// Bibliotheek voor communicatie met I2C / TWI apparaten
#include <Wire.h> 

// Bibliotheek voor het LCD scherm
#include <LiquidCrystal_I2C.h>

/* 
 * Stel hier in welke chip en foromaat LCD je hebt
 * Gebruik 0x27 als je chip PCF8574 hebt van NXP
 * Gebruik 0x3F als je chip PCF8574A hebt van Ti (Texas Instruments)
 * De laatste twee getallen geven het formaat van je LCD aan
 * bijvoorbeeld 20x4 of 16x2
 *  
 */  
LiquidCrystal_I2C lcd(0x27, 16, 2);

/* 
 * Dit druppel-icoon is online gemaakt met de LCD symbool generator op:
 * https://maxpromer.github.io/LCD-Character-Creator/
 */
byte phIcon[] = {
  0x1C,
  0x14,
  0x1C,
  0x10,
  0x14,
  0x14,
  0x07,
  0x05
};

byte degreeSymbol[] = {
  0x07,
  0x05,
  0x07,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00
};

byte temperatureIcon[] = {
  0x04,
  0x0A,
  0x0A,
  0x0A,
  0x0E,
  0x1F,
  0x1F,
  0x0E
};

byte maxIcon[] = {
  0x1F,
  0x04,
  0x0E,
  0x1F,
  0x04,
  0x04,
  0x04,
  0x04
};

byte minIcon[] = {
  0x04,
  0x04,
  0x04,
  0x04,
  0x1F,
  0x0E,
  0x04,
  0x1F
};

bool redrawLCD = true;                  // geeft aan of het scherm moet worden bijgewerkt

//// 1-WIRE DS18B20 SENSOR ////

#include <OneWire.h>
#include <DallasTemperature.h>

static const int oneWireBus = D5;         // 1-wire pin
OneWire oneWire(oneWireBus);              // 1-wire instantie op de oneWireBus pin
DallasTemperature sensors(&oneWire);      // geef de instantie van de oneWire bus door aan de DallasTemperature

float temperatureValue = 0.0;             // variabele om temperatuur in op te slaan (float is getal met komma)
float temperatureMin = 0.0;               // variabele om de laagst gemeten temperatuur in op te slaan
float temperatureMax = 0.0;               // variabele om de hoogst gemeten temperatuur in op te slaan

int interval1Wire = 1000;                 // Tijd in milliseconden tussen het uitlezen van de 1-wire sensor
unsigned long previousMillis1Wire = 0;    // Tijdstip van laatste uitlezing 1-wire sensor


//// ALGEMEEN ////

unsigned long currentMillis = 0;           // Variabele voor het aantal milliseconden sinds de Arduino is gestart

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  // LCD
  lcd.init();                           // initialiseer het LCD scherm
  lcd.backlight();                      // zet de backlight aan
  lcd.createChar(0, phIcon);            // definieer een symbool in geheugen positie 0
  lcd.createChar(1, degreeSymbol);      // definieer een symbool in geheugen positie 1
  lcd.createChar(2, temperatureIcon);   // definieer een symbool in geheugen positie 2
  lcd.createChar(3, maxIcon);           // definieer een symbool in geheugen positie 3
  lcd.createChar(4, minIcon);           // definieer een symbool in geheugen positie 4
  lcd.clear();                          // start met een leeg scherm
  
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //reset settings - for testing
  //wifiManager.resetSettings();

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if(!wifiManager.autoConnect("Aquarino")) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);

  } 

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");

  Blynk.config(auth);

  if(!Blynk.connected()) {
    Blynk.connect();
  }

  // Setup a function to be called every second
  timer.setInterval(1000L, myTimerEvent);

  // Temperature sensor
  sensors.begin();                        // start het uitlezen van de temperatuur sensor

}

// Functie voor het uitlezen van de 1-wire DS18B20 sensor
void read1WireTemp() {

  // Als het tijdsverschil met de vorige uitlezing groter is dan interval1Wire
  if(currentMillis - previousMillis1Wire > interval1Wire) {

    // vraag de temperatuur aan de 1-wire sensor
    sensors.requestTemperatures();

    // neem de temperatuur van de eerste 1-wire sensor
    float readTemperatureValue = sensors.getTempCByIndex(0);
    Serial.println(readTemperatureValue);

    // Controleer of de gelezen waarde anders is dan de vorige waarde
    if(temperatureValue != readTemperatureValue) {
      temperatureValue = readTemperatureValue;
      redrawLCD = true;
      Blynk.virtualWrite(V5, temperatureValue);
    }

    // Controleer of de gelezen waarde groter is dan de opgeslagen max. temperatuur
    if (temperatureValue > temperatureMax) {
        temperatureMax = temperatureValue;
        redrawLCD = true;
        Blynk.virtualWrite(V6, temperatureMax);
    }

    // Controleer of de gelezen waarde groter is dan de opgeslagen min. temperatuur
    if (temperatureValue < temperatureMin || temperatureMin == 0.00) {
        temperatureMin = temperatureValue;
        redrawLCD = true;
        Blynk.virtualWrite(V7, temperatureMin);
    }

    // sla het huidige tijdstip op in previousMillis1Wire
    previousMillis1Wire = currentMillis;

  }

}

void readPhValue() {

  // Als het tijdsverschil met de vorige uitlezing groter is dan intervalDHT
  if(currentMillis - previousMillisPh > intervalPh) {

    // vraag de PH waarde aan de PH sensor
    for(int i=0;i<10;i++) { 
      buf[i]=analogRead(analogInPin);
      delay(30);
    }

    for(int i=0;i<9;i++) {
      for(int j=i+1;j<10;j++) {
        if(buf[i]>buf[j]) {
          temp=buf[i];
          buf[i]=buf[j];
          buf[j]=temp;
        }
      }
    }
  
    avgValue=0;
 
    for(int i=2;i<8;i++)
    avgValue+=buf[i];
    Serial.println(avgValue);
    float pHVol=(float)avgValue*3.3/1023/6;
    float myPhValue = 3.3 * pHVol + calibration;

    // Controleer of de gelezen waarde een getal is (Nan = Not A Number)
    if (isnan(myPhValue)) {
        Serial.println("Uitlezen van PH sensor mislukt!");
        return;
    }

    // Controleer of de gelezen waarde anders is dan de vorige waarde
    if (myPhValue != phValue) {
      phValue = myPhValue;     // werk PhValue bij met de nieuwe waarde
      redrawLCD = true;        // geef aan dat het LCD bijgewerkt moet worden
      Blynk.virtualWrite(V4, phValue);
    }

    previousMillisPh = currentMillis;   // sla het huidige tijdstip op in previousMillisDHT

  }

}

// Functie om het LCD scherm bij te werken
void updateLCD() {
  
  // Als redrawLCD door iets op 'true' is gezet
  if(redrawLCD) {
    
    lcd.clear();                 // wis het scherm
      
    lcd.setCursor(0, 0);         // zet de cursor op positie 1, regel 1
    lcd.write(0);                // teken het ph waarde icoon van geheugenpositie 0
    lcd.print(" ");              // schrijf een spatie
    lcd.print(phValue, 1);          // schrijf de huidige ph waarde

    lcd.setCursor(0, 1);         // zet de cursor op positie 1, regel 2
    lcd.write(2);                // teken het thermometer icoon van geheugenpositie 2
    lcd.print(" ");
    lcd.print(temperatureValue, 1);
    lcd.write(1);                // teken het graden icoon van geheugenpositie 1

    lcd.setCursor(8, 0);        // zet de cursor op positie 11, regel 1
    lcd.write(2);                // teken het thermometer icoon van geheugenpositie 2
    lcd.write(3);
    lcd.print(" ");
    lcd.print(temperatureMax, 1);
    lcd.write(1);                // teken het graden icoon van geheugenpositie 1

    lcd.setCursor(8, 1);        // zet de cursor op positie 11, regel 2
    lcd.write(2);                // teken het thermometer icoon van geheugenpositie 2
    lcd.write(4);
    lcd.print(" ");
    lcd.print(temperatureMin, 1);
    lcd.write(1);                // teken het graden icoon van geheugenpositie 1

    lcd.setCursor(0, 3);
  
    redrawLCD = false;           // set redrawLCD weer op 'false'

  }

}

void loop() {
      // put your main code here, to run repeatedly:
      Blynk.run();
      // timer.run(); // Initiates BlynkTimer

      currentMillis = millis();    // sla de huidige tijd op
      read1WireTemp();             // lees de 1-wire temperatuur sensor uit
      readPhValue();               // lees de Ph sensor uit
      updateLCD();                 // update lcd scherm met actuele waarden
      delay(5000);
}
