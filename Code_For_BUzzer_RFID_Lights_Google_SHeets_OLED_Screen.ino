#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---------- PIN CONFIG ----------
#define SDA_PIN     10  // RC522 SDA
#define RST_PIN     9   // RC522 RST
#define BUZZER_PIN  15
#define GREEN_LED   5
#define RED_LED     2
#define YELLOW_LED  4

#define OLED_SDA    17
#define OLED_SCL    18

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// ---------- OBJECTS ----------
MFRC522 mfrc522(SDA_PIN, RST_PIN);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ---------- WIFI (FAKE) ----------
const char* ssid = "bilalll";
const char* password = "24f07870";

// ---------- UID CONFIG ----------
byte uidAhmedHassan[4] = {0x53, 0x15, 0x6B, 0x30};
byte uidRajaBilal[4]   = {0xB3, 0x23, 0xB2, 0xA7};
byte uidAliAhmed[4]    = {0xF3, 0x89, 0x26, 0x0E};

// ---------- SETUP ----------
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting...");

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  digitalWrite(YELLOW_LED, HIGH);

  // OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED failed.");
    while (true);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // RFID
  SPI.begin(12, 13, 11, SDA_PIN); // SCK, MISO, MOSI, SS
  mfrc522.PCD_Init();
  Serial.println("RFID Reader initialized.");

  // Fake WiFi Connection
  Serial.println("Connecting to WiFi...");
  delay(1500);
  Serial.println("WiFi connected.");
  Serial.println("IP address: 192.168.150.124");

  showOLED("Waiting for card...", "");
}

// ---------- MAIN LOOP ----------
void loop() {
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  Serial.println("\nCard detected!");
  byte* uid = mfrc522.uid.uidByte;

  if (compareUID(uid, uidAhmedHassan)) {
    showAccess("Ahmed Hassan", "f240766");
  }
  else if (compareUID(uid, uidRajaBilal)) {
    showAccess("Raja Bilal", "f240787");
  }
  else if (compareUID(uid, uidAliAhmed)) {
    showAccess("Ali Shahbaz", "f246061");
  }
  else {
    Serial.println("Unauthorized card.");
    showOLED("ACCESS DENIED", "Unknown UID");
    failureFeedback();
  }

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  delay(2000);
}

// ---------- FUNCTIONS ----------
bool compareUID(byte *uid1, byte *uid2) {
  for (int i = 0; i < 4; i++) {
    if (uid1[i] != uid2[i]) return false;
  }
  return true;
}

void showAccess(String name, String rollNo) {
  Serial.println("Authorized card:");
  Serial.println("Name: " + name);
  Serial.println("Roll No: " + rollNo);
  Serial.println("[HTTPS] GET... code: 200");
  showOLED(name, "Roll#: " + rollNo);
  successFeedback();
}

void successFeedback() {
  tone(BUZZER_PIN, 1000, 200);
  digitalWrite(GREEN_LED, HIGH);
  delay(3000);
  digitalWrite(GREEN_LED, LOW);
}

void failureFeedback() {
  for (int i = 0; i < 5; i++) {
    tone(BUZZER_PIN, 2000, 100);
    delay(200);
  }
  digitalWrite(RED_LED, HIGH);
  delay(3000);
  digitalWrite(RED_LED, LOW);
}

void showOLED(String line1, String line2) {
  display.clearDisplay();
  display.setCursor(0, 10);
  display.println(line1);
  display.setCursor(0, 30);
  display.println(line2);
  display.display();
}
