#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 5
#define RST_PIN 4     // RFID RST presunutý z GPIO21 na GPIO4

LiquidCrystal_I2C lcd(0x27, 16, 2); // adresa LCD 0x27, 16x2
MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(9600);

  // I2C na default pinoch LCD
  Wire.begin(21, 22);   // SDA, SCL

  lcd.init();           // inicializácia LCD
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Starting...");

  SPI.begin(18, 19, 23, SS_PIN);  // SCK, MISO, MOSI, SS

  mfrc522.PCD_Init();    // inicializácia RFID
  lcd.setCursor(0, 1);
  lcd.print("Scan a card");
}

void loop() {
  // Ak nie je nová karta → návrat
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  // Prevod UID na hex string
  String uidString = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) uidString += "0";
    uidString += String(mfrc522.uid.uidByte[i], HEX);
  }
  uidString.toLowerCase();

  // Vypis UID na LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("UID:");
  lcd.setCursor(0, 1);
  lcd.print(uidString);

  delay(2000);
  lcd.clear();

  // Porovnanie UID s povolenými kartami
  if (uidString == "e36ac8a6") {
    lcd.print("Access GRANTED");
  }
  else if (uidString == "33fb92aa") {
    lcd.print("Access DENIED");
  }
  else {
    lcd.print("Unknown card");
  }

  delay(2000);
  lcd.clear();
  lcd.print("Scan a card");

  // Stop RFID
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}
