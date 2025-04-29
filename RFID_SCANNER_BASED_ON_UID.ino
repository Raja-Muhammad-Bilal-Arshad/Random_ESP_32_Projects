#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// RFID Pins
#define SS_PIN    9
#define RST_PIN   10

// OLED Display Settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1

// Custom I2C pins for OLED
#define OLED_SDA 18
#define OLED_SCL 8

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
MFRC522 rfid(SS_PIN, RST_PIN);

// Predefined UIDs
byte ahmedHassanUID[4] = {0x53, 0x15, 0x6B, 0x30};
byte rajaBilalUID[4] = {0xB3, 0x23, 0xB2, 0xA7};
byte AliAhmedUID[4] = {0xF3, 0x89, 0x26, 0x0E};


void setup() {
  Serial.begin(115200);

  // Initialize I2C OLED
  Wire.begin(OLED_SDA, OLED_SCL);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Waiting for card...");
  display.display();

  // Initialize SPI for RFID
  SPI.begin(12, 13, 11, 9);
  rfid.PCD_Init();
}

void loop() {
  if ( !rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial() ) {
    return;
  }

  Serial.print("Card UID: ");
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(rfid.uid.uidByte[i], HEX);
  }
  Serial.println();

  // Check UID and display corresponding name
  if (compareUID(rfid.uid.uidByte, ahmedHassanUID)) {
    showMessage("Ahmed Hassan");
  } 
  else if (compareUID(rfid.uid.uidByte, rajaBilalUID)) {
    showMessage("Raja M. Bilal");
  } 
  else if (compareUID(rfid.uid.uidByte, AliAhmedUID)) {
    showMessage("Ali Ahmed");
  } 

  else {
    showMessage("Unknown Card");
  }

  rfid.PICC_HaltA();
}

bool compareUID(byte *uid, byte *knownUID) {
  for (byte i = 0; i < 4; i++) {
    if (uid[i] != knownUID[i]) {
      return false;
    }
  }
  return true;
}

void showMessage(String message) {
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(1);
  display.println("Access Card");
  display.setTextSize(2);
  display.println(message);
  display.display();
  delay(3000);

  // Back to waiting message
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(1);
  display.println("Waiting for card...");
  display.display();
}
