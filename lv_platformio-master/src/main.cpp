#include "BluetoothSerial.h"
#include "sensors.h"
#include "lvgl.h"
#include "ui.h"
#include "app_hal.h"

// Pin Definitions for Sensors
#define NUM_ARCS 3
#define TURBIDITY_SENSOR 34
#define FLOW_SENSOR 14

// Variables for flow sensor
unsigned long Htime, Ltime, Ttime;
float flow_rate, frequency;
// Variables for Turbidity Sensor
long turbidity_raw, water_quality_score;

// Bluetooth object
BluetoothSerial SerialBT;

// Global UI object arrays
lv_obj_t *arcs[NUM_ARCS];
lv_obj_t *value_labels[NUM_ARCS];
lv_obj_t *title_labels[NUM_ARCS];
lv_obj_t *mainUI_Title;
lv_obj_t *mainUI_Subtitle;

void setup() {
  lv_init();
  hal_setup();

  // Setting up Bluetooth and Serial communications
  Serial.begin(115200);
  if (SerialBT.begin("ESP32test")) {
    Serial.println("Bluetooth started");
  } else {
    Serial.println("Bluetooth failed to start");
  }

  // Flow Sensor Initialization
  pinMode(FLOW_SENSOR, INPUT);


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
  SerialBT.println("Hello from ESP32");

  // Turbidity sensor shit
  run_turbidity_sensor();
  SerialBT.println("Turbidity value: ");
  SerialBT.println(water_quality_score);
  Serial.println("Water quality score: ");
  Serial.println(water_quality_score);

  // Flow sensor shit
  run_flow_sensor();
  SerialBT.println("Flow rate: ");
  SerialBT.println(flow_rate);
  Serial.println("Flow rate: ");
  Serial.println(flow_rate);

  update_arc_values();
  lv_task_handler();

  delay(1000);
}
