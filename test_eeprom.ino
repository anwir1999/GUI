#include <EEPROM.h>

#define FLASH_MEMORY_SIZE 1

const int ledPin = 2;
const int buttonPin = 0;

int ledState = 6000;
uint8_t pinForward = 0, forward = 0;
const int debounceDelay = 20;
uint32_t timeDebounceForward = 0;
void setup() { 
  Serial.begin(115200);
  EEPROM.begin(FLASH_MEMORY_SIZE);
  pinMode(buttonPin, INPUT);
  ledState = EEPROM.read(0);
  Serial.println(ledState);
  digitalWrite(ledPin, ledState);
  timeDebounceForward = millis();
}

void loop() {
  pinForward = digitalRead(0);
  if(!pinForward && !forward)
  {
    if(millis() - timeDebounceForward >= 50) 
    {
      forward = 1;
      ledState++;
      timeDebounceForward = millis();
      Serial.println("State changed");
      EEPROM.write(0, ledState);
      if(ledState % 10 == 0)
      {
        EEPROM.commit();       
      }
      Serial.println(ledState);
    }
  }
  if(!pinForward)
  {
     forward = 0;
     timeDebounceForward = millis();
  }
}
