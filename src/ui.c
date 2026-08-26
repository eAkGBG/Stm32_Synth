#include "ui.h"


void synth_ui_init(void){
    //from examples figure this init the screen background color and text color
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x36013f), 0);
    lv_obj_set_style_text_color(screen, lv_color_hex(0xffffff), 0);
    //guess here we add a text label and put it in the center.
    lv_obj_t * label = lv_label_create(screen); //guess we pot it on the screen object
    lv_obj_set_align(label, LV_ALIGN_CENTER);
    lv_label_set_text(label, "Hello Synth");
}