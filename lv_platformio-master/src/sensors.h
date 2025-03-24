#ifndef SENSORS_H
#define SENSORS_H

#ifdef ARDUINO
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#endif

// Function prototypes for sensor readings
void run_flow_sensor();
void run_temp_sensor();
void run_turbidity_sensor();

#endif  // SENSORS_H
