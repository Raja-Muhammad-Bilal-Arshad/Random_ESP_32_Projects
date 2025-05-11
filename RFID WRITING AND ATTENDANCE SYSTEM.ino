#include <SPI.h>
#include <MFRC522.h>
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

// ---------------------------
// PIN CONFIGURATION
/*
Buzzer	 GPIO 15
SDA	     GPIO 10
SCK      GPIO 12
MOSI     GPIO 11
MISO     GPIO 13
RST	     GPIO 9
*/



// ---------------------------
#define SDA_PIN     10  // SS (SDA)
#define RST_PIN     9
#define BUZZER_PIN  15

#define SCK_PIN     12
#define MOSI_PIN    11
#define MISO_PIN    13

MFRC522 mfrc522(SDA_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
MFRC522::StatusCode status;

// ---------------------------
// NETWORK + GOOGLE SCRIPT
// ---------------------------
const char* WIFI_SSID     = "bilalll";
const char* WIFI_PASSWORD = "24f07870";
const String sheet_url = "https://script.google.com/macros/s/AKfycby5EjBt6pC_tIiHP8EytfGDFyDHpkUG3aA94Yet0zP_eR2Hb2H4g-ZGxw_MQ34zpl2j/exec?name=";

// ---------------------------
// RFID CONFIGURATION
// ---------------------------
int blockNum = 2;
byte bufferLen = 18;
byte readBlockData[18];
String card_holder_name;

// ---------------------------
// SETUP
// ---------------------------
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting...");

  // WiFi connection
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Pins
  pinMode(BUZZER_PIN, OUTPUT);

  // SPI + RFID setup
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SDA_PIN); // SCK, MISO, MOSI, SS
  mfrc522.PCD_Init();
  Serial.println("RFID Reader initialized.");
  Serial.println("Scan a card...");
}

// ---------------------------
// MAIN LOOP
// ---------------------------
void loop() {
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  Serial.println("\nCard detected! Reading data...");

  ReadDataFromBlock(blockNum, readBlockData);

  Serial.print("Block ");
  Serial.print(blockNum);
  Serial.print(": ");
  for (int j = 0; j < 16; j++) {
    Serial.write(readBlockData[j]);
  }
  Serial.println();

  // Buzz to confirm
  tone(BUZZER_PIN, 1000, 200);
  delay(250);
  tone(BUZZER_PIN, 1200, 200);
  delay(250);

  // Send to Google Apps Script
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure(); // accept all certificates

    card_holder_name = sheet_url + String((char*)readBlockData);
    card_holder_name.trim();  // clean up extra characters

    Serial.print("[HTTPS] Requesting URL: ");
    Serial.println(card_holder_name);

    HTTPClient https;
    if (https.begin(client, card_holder_name)) {
      int httpCode = https.GET();
      if (httpCode > 0) {
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
      } else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }
      https.end();
    } else {
      Serial.println("[HTTPS] Unable to connect");
    }
  }

  // Halt card so another scan can happen
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  delay(2000);  // Prevent rapid double reads
}

// ---------------------------
// BLOCK READING FUNCTION
// ---------------------------
void ReadDataFromBlock(int blockNum, byte readBlockData[]) {
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Authentication failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = mfrc522.MIFARE_Read(blockNum, readBlockData, &bufferLen);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Read failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
  } else {
    Serial.println("Block read success.");
  }
}
