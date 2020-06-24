/***************************************************
 * Setup routine 
 **************************************************/

void setup() {

  Serial.begin(115200);
  delay(10);

  // Print startup message
  Serial.println("STARTING UP PLANTERINO...");

  Serial.println("Setup WiFi and Blynk");
  setupBlynk();

  Serial.println("Setting up sensors and actuators");
  setupLCD();
  setupDS18B20();

  Serial.println("Setting up config values");
  setupEEPRom();

  Serial.println("Setup is done. Initiating the loop...");
  Serial.println("=====================================");
}
