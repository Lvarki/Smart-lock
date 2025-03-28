#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET     -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

String message = "Welcome! Please scan your card to unlock door.";
int textWidth;
int x = SCREEN_WIDTH;

void setup() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  textWidth = message.length() * 12;
}

void loop() {
  // Create the "viewport" (window)
  display.clearDisplay(); // clear the display
  display.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLACK); // fill the entire screen with black
  display.fillRect(0, 8, SCREEN_WIDTH, 16, WHITE); // Create the white area to display the text

  // Draw the text within the viewport
  display.setTextColor(BLACK); // change the color of the text to black
  display.setCursor(x, 8);
  display.print(message);
  display.display();

  x -= 2.5;
  if (x < -textWidth) {
    x = SCREEN_WIDTH;
  }

  delay(1);
}