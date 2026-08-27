#include "ui.h"

static lv_obj_t * value_label; //file level variable.
static lv_obj_t * label;
static lv_obj_t * screen; //
static int32_t synth_ui_rows[10];
void synth_ui_init(void){
    //from examples figure this init the screen background color and text color
    screen = lv_screen_active();
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x36013f), 0);
    lv_obj_set_style_text_color(screen, lv_color_hex(0xffffff), 0);
    //guess here we add a text label and put it in the center.
    label = lv_label_create(screen); //guess we pot it on the screen object
    value_label = lv_label_create(screen); //asign it once in the init.
    lv_obj_set_align(label, LV_ALIGN_CENTER);
    lv_label_set_text(label, "Hello Synth");
}

//this function is to test if i understand what is needed to print on the screen after ui_init
void synth_ui_test_print(int32_t value){
    //lv_obj_t * screen = lv_screen_active();
    
    char buf[32];
    //lv_snprintf(buf,) //what is this? need to research.
    snprintf(buf, sizeof(buf), "Value=%d", value);
    lv_obj_set_align(value_label, LV_ALIGN_CENTER);
    lv_obj_set_y(value_label, 25);
    lv_label_set_text(value_label, buf);
}