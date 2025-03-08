#include <Arduino.h>
// Library Imports
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h>
#include <RTClib.h>

// Define Constants
#define FLOW_SENSOR 12
#define TEMP_SENSOR 13
#define TURBIDITY_SENSOR 34   // Use an ADC-capable pin
#define BUTTON_PIN 2
#define DEBOUNCE_DELAY 50     // Debounce time in ms

// ESP32 UART2 for Bluetooth
#define RX_PIN 16
#define TX_PIN 17

// Create Objects
HardwareSerial bluetoothSerial(1);  // Use UART1 (RX_PIN = 16, TX_PIN = 17)
LiquidCrystal lcd(22, 23, 5, 18, 19, 21, 4, 2, 15, 14);
OneWire oneWire(TEMP_SENSOR);
DallasTemperature temp_sensor(&oneWire);
RTC_DS1307 rtc;

char daysOfTheWeek[7][12] = {
  "Sunday", "Monday", "Tuesday", "Wednesday",
  "Thursday", "Friday", "Saturday"
};

// Instantiate Variables
int buttonState = 0;
volatile bool buttonPressed = false;
volatile unsigned long lastDebounceTime = 0;

char buffer[16];

unsigned long Htime, Ltime, Ttime;
float flow_rate, frequency, tempC;
long turbidity_raw, water_quality_score;

void IRAM_ATTR buttonISR() {
  unsigned long currentTime = millis();
  if ((currentTime - lastDebounceTime) > DEBOUNCE_DELAY) {
    buttonPressed = true;
    lastDebounceTime = currentTime;
  }
}

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

void push_data() {
  bluetoothSerial.print("Flow: ");
  bluetoothSerial.print(flow_rate);
  bluetoothSerial.print(";");

  if (tempC == -127.00) {
    bluetoothSerial.print("Error: No DS18B20 sensor detected!;");
  } else {
    bluetoothSerial.print("Temperature: ");
    bluetoothSerial.print(tempC);
    bluetoothSerial.print(";");
  }

  bluetoothSerial.print("Turbidity: ");
  bluetoothSerial.print(water_quality_score);
  bluetoothSerial.println("");
}

void print_lcd() {
  lcd.clear();
  switch (buttonState) {
    case 0:
      lcd.home();
      lcd.print("Flow Rate:");
      sprintf(buffer, "%d L/min", (int)flow_rate);
      lcd.setCursor(0, 1);
      lcd.print(buffer);
      break;

    case 1:
      lcd.home();
      lcd.print("Water Quality:");
      sprintf(buffer, "%d/100", water_quality_score);
      lcd.setCursor(0, 1);
      lcd.print(buffer);
      break;

    case 2:
      lcd.home();
      lcd.print("Temperature:");
      sprintf(buffer, "%d C", (int)tempC);
      lcd.setCursor(0, 1);
      lcd.print(buffer);
      break;
  }
}

void generate_datetime() {
  DateTime now = rtc.now();
  bluetoothSerial.printf("Date: %04d-%02d-%02d %02d:%02d:%02d;",
                         now.year(), now.month(), now.day(),
                         now.hour(), now.minute(), now.second());
}

void setup() {
  Serial.begin(115200);
  Serial.println("Setup started!");

  // Initialize Pins
  pinMode(FLOW_SENSOR, INPUT);
  pinMode(TURBIDITY_SENSOR, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.println("Pins initialized!");

  // Interrupt for Button
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);
  Serial.println("Interrupt attached!");

  // Initialize Components
  temp_sensor.begin();
  Serial.println("Temperature sensor initialized!");

  lcd.begin(16, 2);
  lcd.clear();
  Serial.println("LCD initialized!");

  // Start Bluetooth
  bluetoothSerial.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);  // Initialize UART1
  Serial.println("Bluetooth initialized!");

  // Initialize RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    //while (1);  // Program hangs here if RTC fails
  }
  Serial.println("RTC initialized!");

  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  Serial.println("RTC adjusted!");

  Serial.println("Setup complete!");
}

void loop() {
  if (buttonPressed) {
    buttonPressed = false;
    buttonState = (buttonState + 1) % 3;  // Cycle 0-2
  }

  run_flow_sensor();
  run_temp_sensor();
  run_turbidity_sensor();
  print_lcd();
  generate_datetime();
  push_data();

  Serial.println("Sensor Readings:");
  Serial.print("Temperature: ");
  Serial.print(tempC);
  Serial.println(" °C");

  Serial.print("Turbidity: ");
  Serial.print(water_quality_score);
  Serial.println("/100");

  Serial.print("Flow Rate: ");
  Serial.print(flow_rate);
  Serial.println(" L/min");

  Serial.println("---------------------");

  delay(100);
}