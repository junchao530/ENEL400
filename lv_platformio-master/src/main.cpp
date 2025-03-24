/**
  ******************************************************************************
  * @file    main.c
  * @author  Tanveer Parhar
  * @version V1.3
  * @date    02-March-2025
  * @brief   Default main function.
  ******************************************************************************
*/

#include "lvgl.h"
#include "app_hal.h"

#ifdef ARDUINO
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <RTClib.h>
#include <BluetoothSerial.h>
#include <Wire.h>

// DEFINES
#define NUM_ARCS 3
#define FLOW_SENSOR 14
#define TEMP_SENSOR 13
#define TURBIDITY_SENSOR 34   // Use an ADC-capable pin

// GLOBAL VARIABLES

// Arc objects
lv_obj_t *arcs[NUM_ARCS];

// Value labels
lv_obj_t *value_labels[NUM_ARCS];

// Title labels
lv_obj_t *mainUI_Title;
lv_obj_t *mainUI_Subtitle;
lv_obj_t *title_labels[NUM_ARCS];

// Create Objects
BluetoothSerial SerialBT;
OneWire oneWire(TEMP_SENSOR);
DallasTemperature temp_sensor(&oneWire);
RTC_DS1307 rtc;

char buffer[16];

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

String formatSensorData(float tempC, float flow_rate, long turbidity, const String& date_time) {
  // Create a formatted string
  char buffer[100]; // Adjust buffer size as needed
  snprintf(buffer, sizeof(buffer), 
           "Date: %s;Flow: %.2f;Temperature: %.2f;Turbidity: %ld",
           date_time.c_str(), flow_rate, tempC, turbidity);

  return String(buffer);
}

void push_data() {
  DateTime now = rtc.now();
  String date_time = String(now.year()) + "-" + String(now.month()) + "-" + String(now.day()) + " " +
                     String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());

  String sensor_data = formatSensorData(tempC, flow_rate, water_quality_score, date_time);
  
  SerialBT.println(sensor_data);
  Serial.println("Sent: " + sensor_data);
}

// FUNCTION PROTOTYPES
void apply_styles(lv_obj_t *arc, lv_obj_t *title_label, lv_obj_t *value_label);
void generate_flow_arc(void);
void generate_temperature_arc(void);
void generate_quality_arc(void);
void update_arc_values(void);
static void value_changed_event_cb(lv_event_t * e);

// NORMAL ARDUINO
void setup() {
  lv_init();
  hal_setup();
  
  Serial.begin(115200);
  Serial.println("Setup started!");

  // Initialize Pins
  pinMode(FLOW_SENSOR, INPUT);
  pinMode(TURBIDITY_SENSOR, INPUT);
  Serial.println("Pins initialized!");

  // Initialize Components
  temp_sensor.begin();
  Serial.println("Temperature sensor initialized!");

  // Start Bluetooth
  if (!SerialBT.begin("ESP32test #1")) {
    Serial.println("Bluetooth failed to start.");
  } else {
    Serial.println("The device started, now you can pair it with bluetooth!");
  }

  // Initialize RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    //while (1);  // Program hangs here if RTC fails
  }
  Serial.println("RTC initialized!");

  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  Serial.println("RTC adjusted!");

  Serial.println("Setup complete!");

  // Setting the screen background
  lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x16161c), LV_PART_MAIN);

  /* Creating a Title for the Main Screen */
  mainUI_Title = lv_label_create(lv_screen_active());
  lv_label_set_text(mainUI_Title, "HYDROMIND");
  lv_obj_set_style_text_font(mainUI_Title, &lv_font_montserrat_40, LV_PART_MAIN);
  lv_obj_set_style_text_color(mainUI_Title, lv_color_hex(0xffffff), LV_PART_MAIN);
  lv_obj_align(mainUI_Title, LV_ALIGN_TOP_MID, 0, 0);

  /* Creating a Title for the Main Screen */
  mainUI_Subtitle = lv_label_create(lv_screen_active());
  lv_label_set_text(mainUI_Subtitle, "Live Data");
  lv_obj_set_style_text_font(mainUI_Subtitle, &lv_font_montserrat_20, LV_PART_MAIN);
  lv_obj_set_style_text_color(mainUI_Subtitle, lv_color_hex(0xffffff), LV_PART_MAIN);
  lv_obj_align_to(mainUI_Subtitle, mainUI_Title, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

  generate_flow_arc();
  generate_temperature_arc();
  generate_quality_arc();

}

void loop() {
  run_flow_sensor();
  run_temp_sensor();
  run_turbidity_sensor();
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

  update_arc_values();
  lv_task_handler();
  delay(100);
}

// FUNCTION DEFINITIONS

// Function to update arc values
void update_arc_values() {
  static uint8_t flow_value = 0;
  static uint8_t temperature_value = 0;
  static uint8_t quality_value = 0;
  static bool flow_increasing = true;
  static bool temperature_increasing = true;
  static bool quality_increasing = true;

  // Update flow value
  if (flow_increasing) {
      flow_value++;
      if (flow_value >= 100) {
          flow_increasing = false;
      }
  } else {
      flow_value--;
      if (flow_value <= 0) {
          flow_increasing = true;
      }
  }
  lv_arc_set_value(arcs[0], flow_value);
  lv_obj_send_event(arcs[0], LV_EVENT_VALUE_CHANGED, (void*)"L/min");

  // Update temperature value
  if (temperature_increasing) {
      temperature_value++;
      if (temperature_value >= 100) {
          temperature_increasing = false;
      }
  } else {
      temperature_value--;
      if (temperature_value <= 0) {
          temperature_increasing = true;
      }
  }
  lv_arc_set_value(arcs[1], temperature_value);
  lv_obj_send_event(arcs[1], LV_EVENT_VALUE_CHANGED, (void*)"°C");

  // Update quality value
  if (quality_increasing) {
      quality_value++;
      if (quality_value >= 100) {
          quality_increasing = false;
      }
  } else {
      quality_value--;
      if (quality_value <= 0) {
          quality_increasing = true;
      }
  }
  lv_arc_set_value(arcs[2], quality_value);
  lv_obj_send_event(arcs[2], LV_EVENT_VALUE_CHANGED, (void*)"%");
}

void generate_flow_arc(void)
{
    /* Create a title label */
    title_labels[0] = lv_label_create(lv_screen_active());
    lv_label_set_text(title_labels[0], "Flow");
    
    /* Create an Arc */
    arcs[0] = lv_arc_create(lv_screen_active());
    lv_arc_set_bg_angles(arcs[0], 0, 270);
    lv_arc_set_rotation(arcs[0], 135);
    lv_arc_set_value(arcs[0], 0);
    lv_obj_set_size(arcs[0], 125, 125);
    lv_obj_align(arcs[0], LV_ALIGN_CENTER, -150, 25);
    
    /* Create a value label */
    value_labels[0] = lv_label_create(lv_screen_active());
    lv_obj_add_event_cb(arcs[0], value_changed_event_cb, LV_EVENT_VALUE_CHANGED, value_labels[0]);

    apply_styles(arcs[0], title_labels[0], value_labels[0]);
    
    /* Align the title label above the arc */
    lv_obj_align_to(title_labels[0], arcs[0], LV_ALIGN_OUT_TOP_MID, 0, -10);
    
    /* Manually update the value label for the first time */
    lv_obj_send_event(arcs[0], LV_EVENT_VALUE_CHANGED, (void*)"L/min");
}

void generate_temperature_arc(void)
{
    /* Create a title label */
    title_labels[1] = lv_label_create(lv_screen_active());
    lv_label_set_text(title_labels[1], "Temp");
    
    /* Create an Arc */
    arcs[1] = lv_arc_create(lv_screen_active());
    lv_arc_set_bg_angles(arcs[1], 0, 270);
    lv_arc_set_rotation(arcs[1], 135);
    lv_arc_set_value(arcs[1], 0);
    lv_obj_set_size(arcs[1], 125, 125);
    lv_obj_align(arcs[1], LV_ALIGN_CENTER, 0, 25);
    
    /* Create a value label */
    value_labels[1] = lv_label_create(lv_screen_active());
    lv_obj_add_event_cb(arcs[1], value_changed_event_cb, LV_EVENT_VALUE_CHANGED, value_labels[1]);

    apply_styles(arcs[1], title_labels[1], value_labels[1]);
    
    /* Align the title label above the arc */
    lv_obj_align_to(title_labels[1], arcs[1], LV_ALIGN_OUT_TOP_MID, 0, -10);
    
    /* Manually update the value label for the first time */
    lv_obj_send_event(arcs[1], LV_EVENT_VALUE_CHANGED, (void*)"°C");
}

void generate_quality_arc(void)
{
    /* Create a title label */
    title_labels[2] = lv_label_create(lv_screen_active());
    lv_label_set_text(title_labels[2], "Quality");
    
    /* Create an Arc */
    arcs[2] = lv_arc_create(lv_screen_active());
    lv_arc_set_bg_angles(arcs[2], 0, 270);
    lv_arc_set_rotation(arcs[2], 135);
    lv_arc_set_value(arcs[2], 0);
    lv_obj_set_size(arcs[2], 125, 125);
    lv_obj_align(arcs[2], LV_ALIGN_CENTER, 150, 25);
    
    /* Create a value label */
    value_labels[2] = lv_label_create(lv_screen_active());
    lv_obj_add_event_cb(arcs[2], value_changed_event_cb, LV_EVENT_VALUE_CHANGED, value_labels[2]);
    
    apply_styles(arcs[2], title_labels[2], value_labels[2]);

    /* Align the title label above the arc */
    lv_obj_align_to(title_labels[2], arcs[2], LV_ALIGN_OUT_TOP_MID, 0, -10);
    
    /* Manually update the value label for the first time */
    lv_obj_send_event(arcs[2], LV_EVENT_VALUE_CHANGED, (void*)"%");
}

void apply_styles(lv_obj_t *arc, lv_obj_t *title_label, lv_obj_t *value_label) {

  // Arc Styles
  lv_obj_set_style_arc_color(arc, lv_color_hex(0xffffff), LV_PART_MAIN);
  lv_obj_set_style_arc_color(arc, lv_color_hex(0x0e2ee6), LV_PART_INDICATOR);

  // Title Label Styles
  lv_obj_set_style_text_font(title_label, &lv_font_montserrat_30, LV_PART_MAIN);
  lv_obj_set_style_text_color(title_label, lv_color_hex(0xffffff), LV_PART_MAIN);

  // Value Label Styles
  lv_obj_set_style_text_font(value_label, &lv_font_montserrat_30, LV_PART_MAIN);
  lv_obj_set_style_text_color(value_label, lv_color_hex(0xffffff), LV_PART_MAIN);
}

static void value_changed_event_cb(lv_event_t * e)
{
    lv_obj_t * arc = (lv_obj_t*)lv_event_get_target(e);
    lv_obj_t * label = (lv_obj_t*)lv_event_get_user_data(e);
    const char * unit = (const char *)lv_event_get_param(e); // Get the unit from the event parameter

    // Format the label text with the appropriate unit
    lv_label_set_text_fmt(label, "%" LV_PRId32 "%s", lv_arc_get_value(arc), unit);

    /* Align the label to right below the arc */
    lv_obj_align_to(label, arc, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
}

#else

int main(void)
{
	lv_init();

	hal_setup();

  lv_demo_widgets();

	hal_loop();
}

#endif /*ARDUINO*/