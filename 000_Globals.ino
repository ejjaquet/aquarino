/***************************************************
 *        Global settings and pin definitions
 **************************************************/

#define PHSENSORPIN A0
#define ONEWIREBUS D5

unsigned long currentMillis = 0;          // Variabele voor het aantal milliseconden sinds de Arduino is gestart
bool redrawLCD = true;                    // geeft aan of het scherm moet worden bijgewerkt

float temperatureValue = 0.0;             // variabele om temperatuur in op te slaan (float is getal met komma)
float temperatureMin = 0.0;               // variabele om de laagst gemeten temperatuur in op te slaan
float temperatureMax = 0.0;               // variabele om de hoogst gemeten temperatuur in op te slaan

int addrPhThresholdUpper = 0;
float phThresholdUpper;

int addrPhThresholdLower =10;
float phThresholdLower;
