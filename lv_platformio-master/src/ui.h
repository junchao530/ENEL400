#ifndef UI_H
#define UI_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

// Declarations for UI-related functions
void generate_flow_arc(void);
void generate_quality_arc(void);
void update_arc_values(void);
void apply_styles(lv_obj_t *arc, lv_obj_t *title_label, lv_obj_t *value_label);
void value_changed_event_cb(lv_event_t * e);

#ifdef __cplusplus
}
#endif

#endif  // UI_H
