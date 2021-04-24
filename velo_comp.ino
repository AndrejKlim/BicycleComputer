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

uint32_t startTime = millis();
int wheelDiameter = 2100; // in mm
uint32_t totalTime = 0; // in seconds
uint32_t showTimer;

volatile long totalTurnCount = 0;
volatile bool hallActivated;
long prevTurnCount = 0;
uint32_t previousTime = 0;

void displaySpeed(float speed);
void initDisplay();
void incrementTotalTurnCount();
float getCurrentSpeedKmph(uint32_t time);

void setup() {
  Serial.begin(9600);

  initDisplay();
  // D2 это прерывание 0
  // обработчик - функция incrementTotalTurnCount
  attachInterrupt(0, incrementTotalTurnCount, FALLING);

  startTime = millis();
}

void loop() {

  uint32_t turnTime;

  if (hallActivated) {
    hallActivated = false;
    uint32_t ms = millis();
    turnTime = ms - previousTime;
    previousTime = ms;
    
    float currentSpeed = getCurrentSpeedKmph(turnTime);
    displaySpeed(currentSpeed);
    Serial.println("скорость " + (String) currentSpeed);
  }

  delay(5);
}

void initDisplay() {// SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
}

void displaySpeed(float speed) {
  display.clearDisplay();
  display.setTextSize(4);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0, 0);             // Start at top-left corner
  display.println((String) speed);
  display.display();
}

void incrementTotalTurnCount() {
  totalTurnCount++;
  hallActivated = true;
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

// возвращает скорость в км/ч по числу оборотов и времени в мс
float getCurrentSpeedKmph(uint32_t time){
  Serial.println("время в функции: " + (String) time);
  return (wheelDiameter/1000.0f) / (time/1000.0f) * 3.6f;
}
