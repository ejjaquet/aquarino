/***************************************************
 *        Blynk and WiFi related code
 **************************************************/

 void setupBlynk() {
  WiFiManager wifiManager;
  //reset settings - for testing
  //wifiManager.resetSettings();

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);

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
 }

BLYNK_WRITE(V8)
{
  //Executes on V8 change in app
  int pinValue = param.asInt(); // assigning incoming value from pin V8 to a variable
  //Convert to float
  phThresholdUpper = (float)pinValue;
  
  //Update EEPROM now
  uint addr = 0;
  EEPROM.begin(512);
  EEPROM.put(addrPhThresholdUpper, phThresholdUpper);
  Serial.println("Saving...");
  EEPROM.commit(); 
  phThresholdUpper = 0;
  EEPROM.get(addrPhThresholdUpper, phThresholdUpper);
  Serial.println(phThresholdUpper);
}

BLYNK_WRITE(V9)
{
  //Executes on V8 change in app
  int pinValue = param.asInt(); // assigning incoming value from pin V9 to a variable
  //Convert to float
  phThresholdLower = (float)pinValue;
  
  //Update EEPROM now
  uint addr = 0;
  EEPROM.begin(512);
  EEPROM.put(addrPhThresholdLower, phThresholdLower);
  Serial.println("Saving...");
  EEPROM.commit(); 
  phThresholdLower = 0;
  EEPROM.get(addrPhThresholdLower, phThresholdLower);
  Serial.println(phThresholdLower);
}

BLYNK_CONNECTED() {
}
