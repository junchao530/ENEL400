/**
  ******************************************************************************
  * @file    main.c
  * @author  Tanveer Parhar
  * @version V1.3
  * @date    02-March-2025
  * @brief   Default main function.
  ******************************************************************************
*/

#ifdef ARDUINO
#include <Arduino.h>
#include "lvgl.h"
#include "app_hal.h"
#include "sensors.h"
#include "ui.h"
#include "communication.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <RTClib.h>
#include <BluetoothSerial.h>
#include <Wire.h>

// Defines for sensor pins and number of arcs
#define NUM_ARCS 3
#define FLOW_SENSOR      14
#define TEMP_SENSOR      13
#define TURBIDITY_SENSOR 34

// Global UI object arrays
lv_obj_t *arcs[NUM_ARCS];
lv_obj_t *value_labels[NUM_ARCS];
lv_obj_t *title_labels[NUM_ARCS];
lv_obj_t *mainUI_Title;
lv_obj_t *mainUI_Subtitle;

// Global sensor and communication objects
BluetoothSerial SerialBT;
OneWire oneWire(TEMP_SENSOR);
DallasTemperature temp_sensor(&oneWire);
RTC_DS1307 rtc;

// Global variables for sensor data
unsigned long Htime, Ltime, Ttime;
float flow_rate, frequency, tempC;
long turbidity_raw, water_quality_score;

void setup() {
  lv_init();
  hal_setup();
  
  Serial.begin(115200);
  Serial.println("Setup started!");

  pinMode(FLOW_SENSOR, INPUT);
  pinMode(TURBIDITY_SENSOR, INPUT);
  Serial.println("Pins initialized!");

  temp_sensor.begin();
  Serial.println("Temperature sensor initialized!");

  if (!SerialBT.begin("ESP32test #1")) {
    Serial.println("Bluetooth failed to start.");
  } else {
    Serial.println("The device started, now you can pair it with bluetooth!");
  }

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
  }
  Serial.println("RTC initialized!");

  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  Serial.println("RTC adjusted!");

  Serial.println("Setup complete!");

  // Set background color for the screen
  lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x16161c), LV_PART_MAIN);

  // Create main screen title and subtitle
  mainUI_Title = lv_label_create(lv_scr_act());
  lv_label_set_text(mainUI_Title, "HYDROMIND");
  lv_obj_set_style_text_font(mainUI_Title, &lv_font_montserrat_40, LV_PART_MAIN);
  lv_obj_set_style_text_color(mainUI_Title, lv_color_hex(0xffffff), LV_PART_MAIN);
  lv_obj_align(mainUI_Title, LV_ALIGN_TOP_MID, 0, 0);

  mainUI_Subtitle = lv_label_create(lv_scr_act());
  lv_label_set_text(mainUI_Subtitle, "Live Data");
  lv_obj_set_style_text_font(mainUI_Subtitle, &lv_font_montserrat_20, LV_PART_MAIN);
  lv_obj_set_style_text_color(mainUI_Subtitle, lv_color_hex(0xffffff), LV_PART_MAIN);
  lv_obj_align_to(mainUI_Subtitle, mainUI_Title, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

  // Generate UI arcs for flow, temperature, and quality
  generate_flow_arc();
  generate_temperature_arc();
  generate_quality_arc();
}

void loop() {
  // Update sensor readings
  run_flow_sensor();
  run_temp_sensor();
  run_turbidity_sensor();
  
  // Send sensor data over Bluetooth
  push_data();

  // Debug output to Serial Monitor
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

  // Update UI arc values
  update_arc_values();
  lv_task_handler();
  delay(100);
}

#else  // For non-ARDUINO builds

int main(void)
{
	lv_init();
	hal_setup();
	lv_demo_widgets();
	hal_loop();
}

#endif
