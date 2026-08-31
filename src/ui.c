#include "ui.h"

//new synthui init. we vill connect it to the synth struct.
void synth_ui_init(synth_ui_t *synth_ui, synth_data_t *synth){
    synth_ui->active_menu_item = 0;
    synth_ui->bg_color = 0x36013f;
    synth_ui->active_color = 0x8a1fa0;
    synth_ui->text_color = 0xffffff;
    synth_ui->item_count = 0;
    synth_ui->screen = lv_screen_active();
    //setup colors.
    lv_obj_set_style_bg_color(synth_ui->screen, lv_color_hex(synth_ui->bg_color), 0);
    lv_obj_set_style_text_color(synth_ui->screen, lv_color_hex(synth_ui->text_color), 0);
    
    synth_ui_add_item(synth_ui, "Attack", &synth->attack, true);
    synth_ui_add_item(synth_ui, "Decay", &synth->decay, false);

    //int32_t menu_item_size = sizeof(new_item);

}
//function to add menu items.
void synth_ui_add_item(synth_ui_t *synth_ui, char *name, volatile int32_t *value, bool active){
    synth_ui->menu_items = realloc(synth_ui->menu_items, (synth_ui->item_count + 1) * sizeof(synth_ui_item_t));

    synth_ui_item_t *new_item = &synth_ui->menu_items[synth_ui->item_count];
        new_item->name = name;
        new_item->value = value;
        new_item->label = lv_label_create(synth_ui->screen);
        new_item->active = active;
        //lv_obj_set_width(new_item->label, 240 - PADDING_LEFT);
        //lv_obj_set_height(new_item->label, ROW_SIZE);
        //ok soo much happening inside here but i added alot of styling to make the menu pretty.
        //if item set to active it will get a light background.
        //i also created some defines for setting the paddings.
        lv_obj_set_size(new_item->label, 240 - LABEL_PADDING_LEFT, LABEL_ROW_SIZE);
        lv_label_set_long_mode(new_item->label, LV_LABEL_LONG_CLIP);
        lv_label_set_text(new_item->label, new_item->name);
        lv_obj_set_style_pad_left(new_item->label, TEXT_PADDING_LEFT, 0);
        lv_obj_set_style_pad_right(new_item->label, TEXT_PADDING_RIGHT, 0);
        if (active) {
            lv_obj_set_style_bg_color(new_item->label, lv_color_hex(synth_ui->active_color), 0);
            lv_obj_set_style_opa(new_item->label, LV_OPA_100, 0);
            lv_obj_set_style_bg_opa(new_item->label, LV_OPA_COVER, 0);
        }else {
            lv_obj_set_style_bg_color(new_item->label, lv_color_hex(synth_ui->active_color), 0);
            lv_obj_set_style_opa(new_item->label, LV_OPA_40, 0);
            lv_obj_set_style_bg_opa(new_item->label, LV_OPA_COVER, 0);
        }
        lv_obj_set_align(new_item->label, LV_ALIGN_DEFAULT);
        lv_obj_set_x(new_item->label, LABEL_PADDING_LEFT);
        lv_obj_set_y(new_item->label, (synth_ui->item_count * LABEL_ROW_SIZE) + LABEL_PADDING_TOP);
        synth_ui->item_count++;
}

/*
//this function is to test if i understand what is needed to print on the screen after ui_init
void synth_ui_test_print(int32_t value){
    //lv_obj_t * screen = lv_screen_active();
    
    char buf[32];
    //lv_snprintf(buf,) //what is this? need to research.
    snprintf(buf, sizeof(buf), "Value=%d", value);
    //snprintf(buf, sizeof(buf), "This is 24 letters. How?");
    lv_obj_set_align(value_label, LV_ALIGN_CENTER);
    lv_obj_set_y(value_label, 25);
    lv_label_set_text(value_label, buf);
}*/

/* void synth_ui_init(void){
    //from examples figure this init the screen background color and text color
    screen = lv_screen_active();
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x36013f), 0);
    lv_obj_set_style_text_color(screen, lv_color_hex(0xffffff), 0);
    //guess here we add a text label and put it in the center.
    label = lv_label_create(screen); //guess we pot it on the screen object
    value_label = lv_label_create(screen); //asign it once in the init.
    lv_obj_set_align(label, LV_ALIGN_CENTER);
    lv_label_set_text(label, "Hello Synth");
} */