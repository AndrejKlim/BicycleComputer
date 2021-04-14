#define HallSensor 2

int prevHallValue;
uint32_t startTime = millis();
uint32_t previousTime;
uint32_t currentTime;

void setup() {
    startTime = millis();
    pinMode(HallSensor, INPUT);
    Serial.begin(9600);
    previousTime = currentTime;
    prevHallValue = 1;
}

void loop() {    
    if (isHallActivated(prevHallValue)){
        currentTime = millis();
        uint32_t rollTime = currentTime - previousTime;
        previousTime = millis();
        Serial.println("  Время на один оборот " + (String)rollTime);
    }
    prevHallValue = checkHall();
    delay(10);
}

bool isHallActivated(int prevHallValue){
    return (prevHallValue == 1 and checkHall() == 0);
}

int checkHall(){
  return digitalRead(HallSensor);
}
