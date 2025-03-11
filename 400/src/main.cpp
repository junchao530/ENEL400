#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <BluetoothSerial.h>

// Define Constants
#define FLOW_SENSOR 12
#define TEMP_SENSOR 4
#define TURBIDITY_SENSOR 34   // Use an ADC-capable pin

// Create Objects
BluetoothSerial SerialBT;
OneWire oneWire(TEMP_SENSOR);
DallasTemperature temp_sensor(&oneWire);

// Instantiate Variables
unsigned long Htime, Ltime, Ttime;
float flow_rate, frequency, tempC;
long turbidity_raw, water_quality_score;

void run_flow_sensor() {
  Htime = pulseIn(FLOW_SENSOR, HIGH);
  Ltime = pulseIn(FLOW_SENSOR, LOW);
  Ttime = Htime + Ltime;

  if (Ttime > 0) {
    frequency = 1000000.0 / Ttime;
    flow_rate = frequency / 5.5;
  } else {
    flow_rate = 0;  // No flow detected
  }
}

void run_temp_sensor() {
  temp_sensor.requestTemperatures();
  tempC = temp_sensor.getTempCByIndex(0);
}

void run_turbidity_sensor() {
  turbidity_raw = analogRead(TURBIDITY_SENSOR);
  water_quality_score = map(turbidity_raw, 0, 4095, 0, 100);
  water_quality_score = constrain(water_quality_score, 0, 100);
}

String formatSensorData(float tempC, float flow_rate, long turbidity) {
  // Create a formatted string
  char buffer[100]; // Adjust buffer size as needed
  snprintf(buffer, sizeof(buffer), 
           "Flow: %.2f;Temperature: %.2f;Turbidity: %ld",
           flow_rate, tempC, turbidity);

  return String(buffer);
}

void push_data() {
  String sensor_data = formatSensorData(tempC, flow_rate, water_quality_score);
  
  SerialBT.println(sensor_data);
  Serial.println(F("Sent: ")); // Use F() macro for constant strings
  Serial.println(sensor_data);
}

void setup() {
  Serial.begin(115200);
  Serial.println(F("Setup started!")); // Use F() macro for constant strings

  // Initialize Pins
  pinMode(FLOW_SENSOR, INPUT);
  pinMode(TURBIDITY_SENSOR, INPUT);
  Serial.println(F("Pins initialized!")); // Use F() macro for constant strings

  // Initialize Components
  temp_sensor.begin();
  Serial.println(F("Temperature sensor initialized!")); // Use F() macro for constant strings

  // Start Bluetooth
  SerialBT.begin("ESP32test #1"); // Bluetooth device name
  Serial.println(F("The device started, now you can pair it with bluetooth!")); // Use F() macro for constant strings

  Serial.println(F("Setup complete!")); // Use F() macro for constant strings
}

void loop() {
  run_flow_sensor();
  run_temp_sensor();
  run_turbidity_sensor();
  push_data();

  Serial.println(F("Sensor Readings:")); // Use F() macro for constant strings
  Serial.print(F("Temperature: ")); // Use F() macro for constant strings
  Serial.print(tempC);
  Serial.println(F(" °C")); // Use F() macro for constant strings

  Serial.print(F("Turbidity: ")); // Use F() macro for constant strings
  Serial.print(water_quality_score);
  Serial.println(F("/100")); // Use F() macro for constant strings

  Serial.print(F("Flow Rate: ")); // Use F() macro for constant strings
  Serial.print(flow_rate);
  Serial.println(F(" L/min")); // Use F() macro for constant strings

  Serial.println(F("---------------------")); // Use F() macro for constant strings

  // Check for incoming Bluetooth data
  if (SerialBT.available()) {
    String incoming = SerialBT.readString();
    Serial.println(F("Received: ")); // Use F() macro for constant strings
    Serial.println(incoming);
  }

  delay(100);
}