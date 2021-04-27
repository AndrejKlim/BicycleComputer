#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define menuButton 3

int wheelDiameter = 2100; // in mm
uint32_t totalTime = 0; // in seconds
uint32_t showTimer;
uint32_t turnTime;

volatile long totalTurnCount = 0;
volatile bool hallActivated;
long prevTurnCount = 0;
uint32_t previousTime = 0;
volatile byte buttonPressCount = 0;

void displayString(String info);
void displaySpeed(float speed);
void displayTotalTime();
void displayTotalDistance();
void displayAverageSpeed();

void initDisplay();
void incrementTotalTurnCount();
void incrementButtonCLicks();
void setTurnTime();
float getCurrentSpeedKmph(uint32_t time);
void displayChosenInfo();
float getTotalDistance();
float getAverageVelocityKmph();

void setup() {
  Serial.begin(9600);

  initDisplay();
  // D2 это прерывание 0
  // обработчик - функция incrementTotalTurnCount
  attachInterrupt(0, incrementTotalTurnCount, FALLING);
  attachInterrupt(1, incrementButtonCLicks, FALLING);
  pinMode(menuButton, INPUT_PULLUP);
}

void loop() {
  setTurnTime();
  if (millis() - showTimer >= 200){
    showTimer = millis();
    displayChosenInfo();
  }

  delay(5);
}

void setTurnTime(){
  if (hallActivated) {
    hallActivated = false;
    uint32_t ms = millis();
    turnTime = ms - previousTime;
    previousTime = ms;
    totalTime += turnTime;
  }
}

void displayChosenInfo(){
  Serial.println("Menu number " + (String) buttonPressCount);
  switch (buttonPressCount) {
    case 0:
      displaySpeed(getCurrentSpeedKmph(turnTime));
      break;
    case 1:
      displayTotalTime();
      break;
    case 2:
      displayTotalDistance();
      break;
    case 3:
      displayAverageSpeed();
      break;
    default:
      displaySpeed(getCurrentSpeedKmph(turnTime));
      break;
  }
}

void initDisplay() {// SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
}

void displayString(String info, int textSize){
  display.clearDisplay();
  display.setTextSize(textSize);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0, 0);
  display.println(info);
  display.display();
}

void displaySpeed(float speed) {
  Serial.println("Speed " + (String) speed);
  displayString((String) speed, 1);
}

void displayTotalTime(){
  Serial.println("Total time " + (String) totalTime);
  displayString((String) totalTime, 1);
}

void displayTotalDistance(){
  Serial.println("Total distance " + (String) getTotalDistance());
  displayString((String) getTotalDistance(), 1);
}

void displayAverageSpeed(){
  Serial.println("Average speed " + (String) getAverageVelocityKmph());
  displayString((String) getAverageVelocityKmph(), 1);
}

void incrementTotalTurnCount() {
  totalTurnCount++;
  hallActivated = true;
}

void incrementButtonCLicks(){
  buttonPressCount++;
  if (buttonPressCount > 3){
    buttonPressCount = 0;
  }
}

// in meters
float getTotalDistance() {
  return totalTurnCount * (wheelDiameter / 1000.0f);
}

float getAverageVelocityKmph() {
  return getTotalDistance() / (totalTime/1000.0f) * 3.6f;
}

// возвращает скорость в км/ч по числу оборотов и времени в мс
float getCurrentSpeedKmph(uint32_t time){
  Serial.println("время в функции: " + (String) time);
  return (wheelDiameter/1000.0f) / (time/1000.0f) * 3.6f;
}
