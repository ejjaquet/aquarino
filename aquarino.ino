/*
 * Aquarino - A basic aquarium controller for Arduino
 * developed by E.J. Jaquet
 * 
 *
 * PIN AANSLUITINGEN PH SENSOR
 *
 * Vcc  --> 5V  (rood)
 * Data --> A0  (groen)
 * GND  --> GND (zwart)
 * GND --> GND (zwart)
 *
 *
 * PIN AANSLUITINGEN 1-wire SENSOR
 *
 * GND  --> GND (zwart)
 * Data --> D6  (geel)
 * Vcc  --> 5V  (rood)
 *
 * 4K7 (4700) Ohm weerstand tussen 5V en Data pin
 *
 *
 * PIN AANSLUITINGEN LCD I2C
 * 
 * SDA is serial data
 * SCL is serial clock
 *
 * GND --> GND (wit)
 * VCC --> 5V  (grijs)
 * SDA --> D2  (rood)
 * SCL --> D1  (blauw)
 * 
 *  
 */


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


//// PH-SENSOR ////

static const int analogInPin = A0; 
int sensorValue = 0; 
unsigned long int avgValue; 
float b;
int buf[10],temp;

float phValue = 0.0;           // variabele om ph waarde in op te slaan (float is getal met komma)

int intervalPh = 1000;               // Tijd in milliseconden tussen het uitlezen van de ph sensor
unsigned long previousMillisPh = 0;           // Tijdstip van laatste uitlezing ph sensor

//// 1-WIRE DS18B20 SENSOR ////

#include <OneWire.h>
#include <DallasTemperature.h>

static const int oneWireBus = D6;        // 1-wire pin
OneWire oneWire(oneWireBus);            // 1-wire instantie op de oneWireBus pin
DallasTemperature sensors(&oneWire);    // geef de instantie van de oneWire bus door aan de DallasTemperature

float temperatureValue = 0.0;           // variabele om temperatuur in op te slaan (float is getal met komma)
float temperatureMin = 0.0;           // variabele om de laagst gemeten temperatuur in op te slaan
float temperatureMax = 0.0;             // variabele om de hoogst gemeten temperatuur in op te slaan

int interval1Wire = 1000;               // Tijd in milliseconden tussen het uitlezen van de 1-wire sensor
unsigned long previousMillis1Wire = 0;           // Tijdstip van laatste uitlezing 1-wire sensor

//// WIFI (onboard ESP 8266) ////
#include "AdafruitIO_WiFi.h"
 
#define WIFI_SSID       "simplewifi"
#define WIFI_PASS       "92546274353780287453"
 
#define IO_USERNAME    "ejjaquet"
#define IO_KEY         "4347a23da4b7433c9b42f519b2228ab2"
 
// Connect to Wi-Fi and Adafruit IO handel 
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);
 
// Create a feed object that allows us to send data to
AdafruitIO_Feed *phFeed = io.feed("temperature");

//// ALGEMEEN ////

unsigned long currentMillis = 0;        // Variabele voor het aantal milliseconden sinds de Arduino is gestart

//// FUNCTIES ////

void setup() {
  Serial.begin(115200);                 // stel de seriële monitor in
  delay(500);

// Connect to Adafruit IO
  io.connect();
 
  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.println('.');
    delay(500);
  }

  // LCD
  lcd.init();                           // initialiseer het LCD scherm
  lcd.backlight();                      // zet de backlight aan
  lcd.createChar(0, phIcon);            // definieer een symbool in geheugen positie 0
  lcd.createChar(1, degreeSymbol);      // definieer een symbool in geheugen positie 1
  lcd.createChar(2, temperatureIcon);   // definieer een symbool in geheugen positie 2
  lcd.createChar(3, maxIcon);           // definieer een symbool in geheugen positie 3
  lcd.createChar(4, minIcon);           // definieer een symbool in geheugen positie 4
  lcd.clear();                          // start met een leeg scherm

  // Temperature sensor
  sensors.begin();                      // start het uitlezen van de temperatuur sensor

}

// Functie voor het uitlezen van de 1-wire DS18B20 sensor
void read1WireTemp() {

  // Als het tijdsverschil met de vorige uitlezing groter is dan interval1Wire
  if(currentMillis - previousMillis1Wire > interval1Wire) {

    // vraag de temperatuur aan de 1-wire sensor
    sensors.requestTemperatures();

    // neem de temperatuur van de eerste 1-wire sensor
    float readTemperatureValue = sensors.getTempCByIndex(0);

    // Controleer of de gelezen waarde anders is dan de vorige waarde
    if(temperatureValue != readTemperatureValue) {
      temperatureValue = readTemperatureValue;
      redrawLCD = true;
    }

    // Controleer of de gelezen waarde groter is dan de opgeslagen max. temperatuur
    if (temperatureValue > temperatureMax) {
        temperatureMax = temperatureValue;
        redrawLCD = true;
    }

    // Controleer of de gelezen waarde groter is dan de opgeslagen min. temperatuur
    if (temperatureValue < temperatureMin) {
        temperatureMin = temperatureValue;
        redrawLCD = true;
    }

    // sla het huidige tijdstip op in previousMillis1Wire
    previousMillis1Wire = currentMillis;

  }

}

// Functie voor het uitlezen van de DHT sensor
void readPhValue() {

  // Als het tijdsverschil met de vorige uitlezing groter is dan intervalDHT
  if(currentMillis - previousMillisPh > intervalPh) {

    // vraag de PH waarde aan de PH sensor
    for(int i=0;i<10;i++) { 
      buf[i]=analogRead(analogInPin);
      delay(10);
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
    float pHVol=(float)avgValue*5.0/1024/6;
    float myPhValue = -5.70 * pHVol + 21.34;

    // Controleer of de gelezen waarde een getal is (Nan = Not A Number)
    if (isnan(myPhValue)) {
        Serial.println("Uitlezen van PH sensor mislukt!");
        return;
    }

    // Controleer of de gelezen waarde anders is dan de vorige waarde
    if (myPhValue != phValue) {
      phValue = myPhValue;     // werk PhValue bij met de nieuwe waarde
      redrawLCD = true;        // geef aan dat het LCD bijgewerkt moet worden
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
    lcd.print(phValue);          // schrijf de huidige ph waarde

    lcd.setCursor(0, 1);         // zet de cursor op positie 1, regel 2
    lcd.write(2);                // teken het thermometer icoon van geheugenpositie 2
    lcd.print(" ");
    lcd.print(temperatureValue);
    lcd.write(1);                // teken het graden icoon van geheugenpositie 1
    lcd.print("C ");

    lcd.setCursor(10, 0);        // zet de cursor op positie 11, regel 1
    lcd.write(2);                // teken het thermometer icoon van geheugenpositie 2
    lcd.write(3);
    lcd.print(" ");
    lcd.print(temperatureMax);
    lcd.write(1);                // teken het graden icoon van geheugenpositie 1
    lcd.print("C ");

    lcd.setCursor(10, 1);        // zet de cursor op positie 11, regel 2
    lcd.write(2);                // teken het thermometer icoon van geheugenpositie 2
    lcd.write(4);
    lcd.print(" ");
    lcd.print(temperatureMin);
    lcd.write(1);                // teken het graden icoon van geheugenpositie 1
    lcd.print("C ");

    lcd.setCursor(0, 3);
  
    redrawLCD = false;           // set redrawLCD weer op 'false'

  }

}


void loop() {
  io.run();
  currentMillis = millis();    // sla de huidige tijd op
  readPhValue();               // lees de PH sensor uit
  read1WireTemp();             // lees de 1-wire temperatuur sensor uit
  updateLCD();                 // werk indien nodig het LCD scherm bij
  phFeed->save(phValue);
  delay(5000);
}
