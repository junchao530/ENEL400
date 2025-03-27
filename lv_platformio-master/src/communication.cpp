#ifdef ARDUINO
#include "communication.h"
#include "RTClib.h"
#include "BluetoothSerial.h"

// External declarations for globals defined in main.cpp
extern RTC_DS1307 rtc;
extern BluetoothSerial SerialBT;
extern float flow_rate;
extern long water_quality_score;

String formatSensorData(float flow_rate, long turbidity, const String& date_time) {
  char buffer[100];
  snprintf(buffer, sizeof(buffer),
           "Date: %s;Flow: %.2f;Turbidity: %ld",
           date_time.c_str(), flow_rate, turbidity);
  return String(buffer);
}

void push_data() {
  DateTime now = rtc.now();
  String date_time = String(now.year()) + "-" + String(now.month()) + "-" +
                       String(now.day()) + " " + String(now.hour()) + "-" +
                       String(now.minute()) + "-" + String(now.second());

  String sensor_data = formatSensorData(flow_rate, water_quality_score, date_time);
  SerialBT.println(sensor_data);
  Serial.println("Sent: " + sensor_data);
}
#endif
