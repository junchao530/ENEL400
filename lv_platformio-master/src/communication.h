#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#ifdef ARDUINO
#include <Arduino.h>
#include <RTClib.h>
#include <BluetoothSerial.h>
#endif

// Declarations for formatting sensor data and pushing it out via Bluetooth
String formatSensorData(float tempC, float flow_rate, long turbidity, const String& date_time);
void push_data();

#endif  // COMMUNICATION_H
