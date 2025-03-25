#ifdef ARDUINO
#include "sensors.h"
#include "app_hal.h"

// Sensor pin definitions
#define FLOW_SENSOR      14
#define TURBIDITY_SENSOR 34

// Declare external globals (these are defined in main.cpp)
extern unsigned long Htime, Ltime, Ttime;
extern float flow_rate, frequency;
extern long turbidity_raw, water_quality_score;

void run_flow_sensor() {
  Htime = pulseIn(FLOW_SENSOR, HIGH);
  Ltime = pulseIn(FLOW_SENSOR, LOW);
  Ttime = Htime + Ltime;

  if (Ttime > 0) {
    frequency = 1000000.0 / Ttime;
    flow_rate = frequency / 5.5;
  } else {
    flow_rate = 0;
  }
}

void run_turbidity_sensor() {
  turbidity_raw = analogRead(TURBIDITY_SENSOR);
  water_quality_score = map(turbidity_raw, 0, 4095, 0, 100);
  water_quality_score = constrain(water_quality_score, 0, 100);
}
#endif
