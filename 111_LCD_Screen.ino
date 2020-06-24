/***************************************************
 * LCD Screen with i2c interface
 * 
 * Pinout: 
 * 
 * SDA is serial data
 * SCL is serial clock
 *
 * GND --> GND
 * VCC --> 5V
 * SDA --> D2
 * SCL --> D1  
 **************************************************/

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

void setupLCD() {
  lcd.init();                           // initialiseer het LCD scherm
  lcd.backlight();                      // zet de backlight aan
  lcd.createChar(0, phIcon);            // definieer een symbool in geheugen positie 0
  lcd.createChar(1, degreeSymbol);      // definieer een symbool in geheugen positie 1
  lcd.createChar(2, temperatureIcon);   // definieer een symbool in geheugen positie 2
  lcd.createChar(3, maxIcon);           // definieer een symbool in geheugen positie 3
  lcd.createChar(4, minIcon);           // definieer een symbool in geheugen positie 4
  lcd.clear();                          // start met een leeg scherm
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
