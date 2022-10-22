// COMP-10184 – Mohawk College 
// AlarmSystem
// 
// @author  Jaspal Marok
// @id   000381777
// 
// I created this work and I have not shared it with anyone else. 
// 
 
#include <Arduino.h> 
 
#define PIN_PIR D5 
#define PIN_BUTTON D6 

#define BUTTON_DELAY_MS 200
#define LED_BLINK_INTERVAL_MS 200
#define MAX_ALARM_INTERVENTION_MS 10000

#define ALARM_DISABLED 0
#define ALARM_ENABLED 1
#define ALARM_COUNTDOWN 2
#define ALARM_ACTIVE 3

int alarmState;
int ledState;
unsigned long triggerAlarmAt;
unsigned long ledLastToggledTimestampMs;

bool isSensorActive;
bool isSwitchOpen;
 

// ************************************************************* 
void setup() { 
  Serial.begin(115200); 
 
  //Set PIN MODES
  pinMode(LED_BUILTIN, OUTPUT); 
  pinMode(PIN_PIR, INPUT); 
  pinMode(PIN_BUTTON, INPUT_PULLUP); 

  //Defaults
  alarmState = ALARM_ENABLED;
  ledState = HIGH;
  ledLastToggledTimestampMs = 0;
} 

// Get text value for alarm state - log helper
String getTextAlarmState(int currentState)
{
  String state;
  switch (currentState)
  {
    case ALARM_DISABLED:
      state = "ALARM_DISABLED";
      break;
    case ALARM_ENABLED:
      state = "ALARM_ENABLED";
      break;
    case ALARM_COUNTDOWN:
      state = "ALARM_COUNTDOWN";
      break;
    case ALARM_ACTIVE:
      state = "ALARM_ACTIVE";
      break;
    default:
      state = "UNKNOWN";
      break;
  }
  return state;
}

// Change alarm state and log
void setAlarmState(int newAlarmState)
{

   Serial.println("\nChanging alarm state from: " + getTextAlarmState(alarmState) + " to: " + getTextAlarmState(newAlarmState));
   alarmState = newAlarmState;
}

// Change led state and log
void setLedState(int newLedState)
{
  ledState = newLedState;
  digitalWrite(LED_BUILTIN, ledState);
}

// Get digital value for sensor
void captureSensor()
{
  isSensorActive = digitalRead(PIN_PIR);
}

// Get digital value for button
void captureButton()
{
   isSwitchOpen = digitalRead(PIN_BUTTON);
   delay(BUTTON_DELAY_MS);
}

// Check state of alarm and change state
void alarmStateCheck()
{
  switch (alarmState)
  {
    case ALARM_ENABLED:
      if(isSensorActive){
          setAlarmState(ALARM_COUNTDOWN);
          break;
      }
      break;
    case ALARM_DISABLED:
      if(!isSwitchOpen){
        setAlarmState(ALARM_ENABLED);
        break;
      }
      break;
    case ALARM_COUNTDOWN:
      //Check if alarm is disabled within 10 seconds
      triggerAlarmAt = millis() + MAX_ALARM_INTERVENTION_MS;
      while (millis() < triggerAlarmAt)
      {
        captureButton();
        if(!isSwitchOpen){
          setAlarmState(ALARM_DISABLED);
          setLedState(HIGH);
          break;
        }

        unsigned long currentTimestampMs = millis();
        if (currentTimestampMs - ledLastToggledTimestampMs >= LED_BLINK_INTERVAL_MS) {
          ledLastToggledTimestampMs = millis();
          setLedState(!ledState);
        }
      }

      // Check if while loop disabled the alarm;
      if(alarmState != ALARM_DISABLED)
      {
        //Alarm not disabled, within time-limit.
        //Activate alarm, led on
        setAlarmState(ALARM_ACTIVE);
        setLedState(LOW);
      }      
      break;
    case ALARM_ACTIVE:
      Serial.println("Alarmmmmm - Beep Beep Beep!");
      break;
    default:
       Serial.println("Something went wrong :(");
      break;
  }
}
 
// ************************************************************* 
void loop() { 
  setLedState(ledState);
  captureSensor();
  captureButton();
  alarmStateCheck();
}