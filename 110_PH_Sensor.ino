/***************************************************
 * PH Sensor
 * Pinout: 
 * Vcc  --> 5V
 * Data --> A0
 * GND  --> GND
 * GND --> GND
 * 
 * Measures the ph value of any liquid
 **************************************************/
 
int sensorValue = 0; 
unsigned long int avgValue;
float calibration = -0.50;               //change this value to calibrate
float b;
int buf[10],temp;

float phValue = 0.0;                    // variabele om ph waarde in op te slaan (float is getal met komma)

int intervalPh = 1000;                  // Tijd in milliseconden tussen het uitlezen van de ph sensor
unsigned long previousMillisPh = 0;     // Tijdstip van laatste uitlezing ph sensor

void readPhValue() {

  // Als het tijdsverschil met de vorige uitlezing groter is dan intervalDHT
  if(currentMillis - previousMillisPh > intervalPh) {

    // vraag de PH waarde aan de PH sensor
    for(int i=0;i<10;i++) { 
      buf[i]=analogRead(PHSENSORPIN);
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
