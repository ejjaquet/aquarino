/***************************************************
 * 1-WIRE DS18B20 SENSOR
 * Pinout: 
 * 
 * GND  --> GND
 * Data --> D7
 * Vcc  --> 5V
 * 
 * Measures the temperature of any fluid
 **************************************************/

OneWire oneWire(ONEWIREBUS);              // 1-wire instantie op de oneWireBus pin
DallasTemperature sensors(&oneWire);      // geef de instantie van de oneWire bus door aan de DallasTemperature

int interval1Wire = 1000;                 // Tijd in milliseconden tussen het uitlezen van de 1-wire sensor
unsigned long previousMillis1Wire = 0;    // Tijdstip van laatste uitlezing 1-wire sensor

void setupDS18B20() {
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
