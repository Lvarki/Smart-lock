// 📡 RFID START
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 7
MFRC522 rfid(SS_PIN, RST_PIN); // RFID instance

// 🖥️ OLED START
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// 🎛 Adjustable Variables
#define RELAY_PIN 5        // Relay connected to pin 5
#define RELAY_TIME 3000    // Time relay stays ON (milliseconds)
#define SCROLL_SPEED 4     // Scrolling text speed (higher = faster)
#define FRAME_RATE 10      // OLED refresh rate (milliseconds)

// Allowed RFID UID (Replace with your actual card UID)
byte allowedUID1[] = {0xBE, 0x7A, 0xF8, 0x03};  

String welcomeMessage = "Welcome! Scan your card.";
String accessDeniedMessage = "Access Denied!";
int textWidth;
int x = SCREEN_WIDTH;

void setup() {
  Serial.begin(9600);
  Serial.println("System Starting...");

  // OLED INIT
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED allocation failed"));
    while (true);
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  textWidth = welcomeMessage.length() * 12;

  // RFID INIT
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("RFID Ready.");

  // Relay INIT
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);  // Ensure relay starts off
}

void loop() {
  static unsigned long lastUpdateTime = 0;
  unsigned long currentTime = millis();

  // OLED SCROLLING MESSAGE
  if (currentTime - lastUpdateTime >= FRAME_RATE) {
    display.clearDisplay();
    display.fillRect(0, 8, SCREEN_WIDTH, 16, WHITE);
    display.setTextColor(BLACK);
    display.setCursor(x, 8);
    display.print(welcomeMessage);
    display.display();

    x -= SCROLL_SPEED;
    if (x < -textWidth) x = SCREEN_WIDTH;

    lastUpdateTime = currentTime;
  }

  // 📡 Check for new RFID card
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return;

  Serial.println("\n🔍 Card Detected!");
  Serial.print("UID: ");
  printHex(rfid.uid.uidByte, rfid.uid.size);
  Serial.println();

  if (checkUID(rfid.uid.uidByte, rfid.uid.size)) {
    Serial.println("✅ Authorized card detected. Unlocking door...");
    unlockDoor();
    Serial.println("🔒 Door lock process completed.");
  } else {
    Serial.println("❌ Unauthorized card. Access Denied.");
    scrollMessage(accessDeniedMessage);
  }

  // 📡 Ensure RFID module doesn't get stuck
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  rfid.PCD_Init();  // Reinitialize the RFID reader
  Serial.println("RFID scan cycle complete. Waiting for new card...");
}

// 🚪 Unlocks the door by activating the relay and scrolling "Door Unlocked!"
void unlockDoor() {
  Serial.println("🚪 Unlocking door...");
  digitalWrite(RELAY_PIN, HIGH);  // Turn relay ON
  unsigned long unlockStartTime = millis();
  int msgWidth = 12 * 14;  // Estimated message width
  int xScroll = SCREEN_WIDTH;

  while (millis() - unlockStartTime < RELAY_TIME) {
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setCursor(xScroll, 8);
    display.print("Door Unlocked!");
    display.display();

    xScroll -= SCROLL_SPEED;
    if (xScroll < -msgWidth) xScroll = SCREEN_WIDTH;

    delay(10);
  }

  digitalWrite(RELAY_PIN, LOW);   // Turn relay OFF
  Serial.println("🔒 Door locked again.");
}

// 🆔 Checks if the scanned UID matches the stored UID
bool checkUID(byte *scannedUID, byte uidSize) {
  return (uidSize == sizeof(allowedUID1) && memcmp(scannedUID, allowedUID1, uidSize) == 0);
}

// ⛔ Scrolls "Access Denied" across OLED screen
void scrollMessage(String message) {
  int msgWidth = message.length() * 12;
  int xScroll = SCREEN_WIDTH;

  while (xScroll > -msgWidth) {
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setCursor(xScroll, 8);
    display.print(message);
    display.display();
    xScroll -= 3;
    delay(10);
  }
}

// 🔢 Print RFID UID in hex format
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}
