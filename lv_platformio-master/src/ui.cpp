#include "ui.h"
#include "lvgl.h"

// External declarations for UI objects (global arrays defined in main.cpp)
extern lv_obj_t *arcs[];
extern lv_obj_t *value_labels[];
extern lv_obj_t *title_labels[];
extern float flow_rate;
extern long water_quality_score;

void generate_flow_arc(void) {
  title_labels[0] = lv_label_create(lv_scr_act());
  lv_label_set_text(title_labels[0], "Flow");
  
  arcs[0] = lv_arc_create(lv_scr_act());
  lv_arc_set_bg_angles(arcs[0], 0, 270);
  lv_arc_set_rotation(arcs[0], 135);
  lv_arc_set_range(arcs[0], 0, 30);
  lv_arc_set_value(arcs[0], 0);
  lv_obj_set_size(arcs[0], 125, 125);
  lv_obj_align(arcs[0], LV_ALIGN_CENTER, 100, 0);
  
  value_labels[0] = lv_label_create(lv_scr_act());
  lv_obj_add_event_cb(arcs[0], value_changed_event_cb, LV_EVENT_VALUE_CHANGED, value_labels[0]);

  apply_styles(arcs[0], title_labels[0], value_labels[0]);
  lv_obj_align_to(title_labels[0], arcs[0], LV_ALIGN_OUT_TOP_MID, 0, -10);
  lv_obj_send_event(arcs[0], LV_EVENT_VALUE_CHANGED, (void*)"L/min");
}

void generate_quality_arc(void) {
  title_labels[1] = lv_label_create(lv_scr_act());
  lv_label_set_text(title_labels[1], "Quality");
  
  arcs[1] = lv_arc_create(lv_scr_act());
  lv_arc_set_bg_angles(arcs[1], 0, 270);
  lv_arc_set_rotation(arcs[1], 135);
  lv_arc_set_value(arcs[1], 0);
  lv_obj_set_size(arcs[1], 125, 125);
  lv_obj_align(arcs[1], LV_ALIGN_CENTER, -100, 0);
  
  value_labels[1] = lv_label_create(lv_scr_act());
  lv_obj_add_event_cb(arcs[1], value_changed_event_cb, LV_EVENT_VALUE_CHANGED, value_labels[1]);
  
  apply_styles(arcs[1], title_labels[1], value_labels[1]);
  lv_obj_align_to(title_labels[1], arcs[1], LV_ALIGN_OUT_TOP_MID, 0, -10);
  lv_obj_send_event(arcs[1], LV_EVENT_VALUE_CHANGED, (void*)"%");
}

void update_arc_values(void) {
  static uint8_t temperature_value = 0;
  static bool temperature_increasing = true;

  // Update flow arc value
  lv_arc_set_value(arcs[0], flow_rate);
  lv_obj_send_event(arcs[0], LV_EVENT_VALUE_CHANGED, (void*)"L/min");

  // Update quality arc value
  lv_arc_set_value(arcs[1], water_quality_score);
  lv_obj_send_event(arcs[1], LV_EVENT_VALUE_CHANGED, (void*)"%");
}

void apply_styles(lv_obj_t *arc, lv_obj_t *title_label, lv_obj_t *value_label) {
  lv_obj_set_style_arc_color(arc, lv_color_hex(0xffffff), LV_PART_MAIN);
  lv_obj_set_style_arc_color(arc, lv_color_hex(0x0e2ee6), LV_PART_INDICATOR);
  
  lv_obj_set_style_text_font(title_label, &lv_font_montserrat_30, LV_PART_MAIN);
  lv_obj_set_style_text_color(title_label, lv_color_hex(0xffffff), LV_PART_MAIN);
  
  lv_obj_set_style_text_font(value_label, &lv_font_montserrat_30, LV_PART_MAIN);
  lv_obj_set_style_text_color(value_label, lv_color_hex(0xffffff), LV_PART_MAIN);
}

void value_changed_event_cb(lv_event_t * e) {
  lv_obj_t * arc = (lv_obj_t*)lv_event_get_target(e);
  lv_obj_t * label = (lv_obj_t*)lv_event_get_user_data(e);
  const char * unit = (const char *)lv_event_get_param(e);
  
  lv_label_set_text_fmt(label, "%" LV_PRId32 "%s", lv_arc_get_value(arc), unit);
  lv_obj_align_to(label, arc, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
}
