/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include "lvgl.h"
#include "app_hal.h"

#ifdef ARDUINO
#include <Arduino.h>

// DEFINES
#define NUM_ARCS 3

// GLOBAL VARIABLES

// Arc objects
lv_obj_t *arcs[NUM_ARCS];

//
lv_obj_t *labels[NUM_ARCS];

// FUNCTION PROTOTYPES
void update_arc_values(void);
void generate_arcs(void);
static void value_changed_event_cb(lv_event_t * e);

// NORMAL ARDUINO
void setup() {
  lv_init();
  hal_setup();
  generate_arcs();
}

void loop() {
  update_arc_values();
  lv_task_handler();
  delay(10);
}

// FUNCTION DEFINITIONS

// Function to update arc values
void update_arc_values() {
  static uint8_t values[NUM_ARCS] = {0, 0, 0};
  static bool increasing[NUM_ARCS] = {true, true, true};

  for (int i = 0; i < NUM_ARCS; i++) {
      if (increasing[i]) {
          values[i]++;
          if (values[i] >= 100) {
              increasing[i] = false;
          }
      } else {
          values[i]--;
          if (values[i] <= 0) {
              increasing[i] = true;
          }
      }
      lv_arc_set_value(arcs[i], values[i]);
      lv_obj_send_event(arcs[i], LV_EVENT_VALUE_CHANGED, NULL);
  }
}
void generate_arcs(void)
{
    for (int i = 0; i < NUM_ARCS; i++) {
      labels[i] = lv_label_create(lv_screen_active());
      arcs[i] = lv_arc_create(lv_screen_active());
      lv_arc_set_bg_angles(arcs[i], 0, 270);
      lv_arc_set_rotation(arcs[i], 135);
      lv_arc_set_value(arcs[i], 0);
      lv_obj_set_size(arcs[i], 100, 100);
      if (i == 0) {lv_obj_align(arcs[i], LV_ALIGN_CENTER, 0, 0);}
      else if (i == 1) {lv_obj_align_to(arcs[i], arcs[0], LV_ALIGN_OUT_LEFT_MID, -50, 0);}
      else {lv_obj_align_to(arcs[i], arcs[0], LV_ALIGN_OUT_RIGHT_MID, 50, 0);}
      
      lv_obj_add_event_cb(arcs[i], value_changed_event_cb, LV_EVENT_VALUE_CHANGED, labels[i]);
      lv_obj_send_event(arcs[i], LV_EVENT_VALUE_CHANGED, NULL);
    }

    // lv_obj_t * label = lv_label_create(lv_screen_active());

    // /*Create an Arc*/
    // lv_obj_t * arc = lv_arc_create(lv_screen_active());
    // lv_obj_set_size(arc, 100, 100);
    // lv_arc_set_rotation(arc, 135);
    // lv_arc_set_bg_angles(arc, 0, 270);
    // lv_arc_set_value(arc, 10);
    // lv_obj_set_pos(arc, 30, 220);
    // lv_obj_add_event_cb(arc, value_changed_event_cb, LV_EVENT_VALUE_CHANGED, label);

    // /*Manually update the label for the first time*/
    // lv_obj_send_event(arc, LV_EVENT_VALUE_CHANGED, NULL);
}

static void value_changed_event_cb(lv_event_t * e)
{
    lv_obj_t * arc = (lv_obj_t*)lv_event_get_target(e);
    lv_obj_t * label = (lv_obj_t*)lv_event_get_user_data(e);

    lv_label_set_text_fmt(label, "%" LV_PRId32 "%%", lv_arc_get_value(arc));

    lv_obj_align_to(label, arc, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    // /*Rotate the label to the current position of the arc*/
    // lv_arc_rotate_obj_to_angle(arc, label, 25);
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