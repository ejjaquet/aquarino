void setupEEPRom() {
  EEPROM.begin(512);

  EEPROM.get(addrPhThresholdUpper,phThresholdUpper);  //Get the upper ph threshold
  EEPROM.get(addrPhThresholdLower,phThresholdLower);  //Get the upper ph threshold

  Serial.println("Syncing with Blynk...");
  delay(500);
  Blynk.syncAll();
  delay(500);
  Serial.println("Done!");
}
