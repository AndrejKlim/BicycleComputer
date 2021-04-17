#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define HallSensor 2

int prevHallValue;
uint32_t startTime = millis();
uint32_t previousTime;
uint32_t currentTime;
int wheelDiameter = 2100; // in mm
uint32_t totalTime = 0; // in seconds
uint32_t totalTurnCount = 0;

void displaySpeed(const uint32_t &rollTime);

void initDisplay();

void setup() {
  Serial.begin(9600);

  pinMode(HallSensor, INPUT);
  initDisplay();

  startTime = millis();
  previousTime = currentTime;
  prevHallValue = 1;
}

void loop() {
  if (isHallActivated(prevHallValue)) {
    uint32_t rollTime = getTurnTimeMs();
    Serial.println("Время на один оборот " + (String) rollTime);

    displaySpeed(rollTime);
  }

  prevHallValue = checkHall();
  delay(10);
}

void initDisplay() {// SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
}

void displaySpeed(const uint32_t rollTime) {
  display.clearDisplay();
  display.setTextSize(6);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0, 0);             // Start at top-left corner
  display.println((String) rollTime);
  display.display();
}


bool isHallActivated(int previousHallValue) {
  return (previousHallValue == 1 and checkHall() == 0);
}

uint32_t getTurnTimeMs() {
  currentTime = millis();
  uint32_t rollTime = currentTime - previousTime;
  previousTime = currentTime;
  return rollTime;
}

int checkHall() {
  return digitalRead(HallSensor);
}

void incrementTotalTurnCount() {
  totalTurnCount++;
}

int getTotalPath() {
  return totalTurnCount / wheelDiameter;
}

int getAverageVelocity() {
  return getTotalPath() / totalTime;
}

float roundAverageVelocity() {
  return getAverageVelocity();
}
