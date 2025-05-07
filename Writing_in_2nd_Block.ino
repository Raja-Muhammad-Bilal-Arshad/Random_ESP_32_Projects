#include <SPI.h>
#include <MFRC522.h>

// Pin configuration
#define RST_PIN   10  // GPIO10
#define SS_PIN    21  // SDA
#define SCK_PIN   36  // SCK
#define MISO_PIN  37  // MISO
#define MOSI_PIN  35  // MOSI

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

int blockNum = 2;
byte blockData[16] = { "Mohsin Shoaib" };
byte bufferLen = 18;
byte readBlockData[18];
MFRC522::StatusCode status;

void setup() {
  Serial.begin(115200);

  // Use custom pins for SPI bus
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);

  // Initialize RFID module with SS and RST only
  mfrc522.PCD_Init(SS_PIN, RST_PIN);
  Serial.println("Scan RFID Card...");
}

void loop() {
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) return;

  Serial.println("\n**Card Detected**");
  Serial.print("UID: ");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println();

  Serial.println("\nWriting data to card...");
  WriteDataToBlock(blockNum, blockData);

  Serial.println("\nReading data back...");
  ReadDataFromBlock(blockNum, readBlockData);

  Serial.print("Data from Block ");
  Serial.print(blockNum);
  Serial.print(": ");
  for (int j = 0; j < 16; j++) {
    Serial.write(readBlockData[j]);
  }
  Serial.println();
}

void WriteDataToBlock(int blockNum, byte blockData[]) {
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Auth failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = mfrc522.MIFARE_Write(blockNum, blockData, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Write failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  Serial.println("Write success.");
}

void ReadDataFromBlock(int blockNum, byte readBlockData[]) {
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Auth failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = mfrc522.MIFARE_Read(blockNum, readBlockData, &bufferLen);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Read failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  Serial.println("Read success.");
}
