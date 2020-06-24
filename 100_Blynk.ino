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
