/***************************************************
 * The actual execution of the code, 
 * pauses for 5 seconds and continues
 **************************************************/

void loop() {
      Blynk.run();

      currentMillis = millis();    // sla de huidige tijd op
      read1WireTemp();             // lees de 1-wire temperatuur sensor uit
      readPhValue();               // lees de Ph sensor uit
      checkPHValue();              // check dePH waarde en stuur ios notificatie indien niet ok
      updateLCD();                 // update lcd scherm met actuele waarden
      Serial.println("=====================================");
      delay(5000);
}
