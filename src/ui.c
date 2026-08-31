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
    synth_ui_add_item(synth_ui, "Sustain", &synth->sustain, false);
    synth_ui_add_item(synth_ui, "Release", &synth->release, false);
    //built in function pointers
    synth_ui->set_value = synth_ui_set_value; 
    synth_ui->update_menu = synth_ui_update_menu;
    synth_ui->nav_next = synth_ui_nav_next;
    synth_ui->nav_prev = synth_ui_nav_prev;
    synth_ui->val_up = synth_ui_val_up;
    synth_ui->val_down = synth_ui_val_down;
}

//function to set a new synth ui item value
void synth_ui_set_value(synth_ui_t *synth_ui, int32_t value){
    *synth_ui->menu_items[synth_ui->active_menu_item].value = value;
}
//function to refresh the menu. Used after we change somethig.
void synth_ui_update_menu(synth_ui_t *synth_ui){
    for(int32_t i = 0; i < synth_ui->item_count; i++){
        if(synth_ui->active_menu_item == i){
            synth_ui->menu_items[i].active = true;
            lv_obj_set_style_opa(synth_ui->menu_items[i].label, LV_OPA_100, 0);
            lv_obj_set_style_bg_opa(synth_ui->menu_items[i].label, LV_OPA_COVER, 0);
            lv_obj_set_style_text_color(synth_ui->menu_items[i].label, lv_color_hex(synth_ui->text_color), 0);
            lv_label_set_text_fmt(synth_ui->menu_items[i].label,"%-15s:%d", synth_ui->menu_items[i].name, (int)*synth_ui->menu_items[i].value);
        }else{
            synth_ui->menu_items[i].active = false;
            lv_obj_set_style_opa(synth_ui->menu_items[i].label, LV_OPA_40, 0);
            lv_obj_set_style_bg_opa(synth_ui->menu_items[i].label, LV_OPA_COVER, 0);

            lv_label_set_text_fmt(synth_ui->menu_items[i].label,"%-15s:%d", synth_ui->menu_items[i].name, (int)*synth_ui->menu_items[i].value);
        }
    }
}
//navigation function for next menu item. i think i will drop the synth_ui naming is simply annoying to type.
void synth_ui_nav_next(synth_ui_t *ui){
    if(ui->active_menu_item < ui->item_count - 1){
        ui->active_menu_item++;
        //ui->update_menu(ui);
    }
    else if (ui->active_menu_item == ui->item_count - 1)
    {
        ui->active_menu_item = 0;
        //ui->update_menu(ui);
    }
    
}
void synth_ui_nav_prev(synth_ui_t *ui){
    if(ui->active_menu_item > 0){
        ui->active_menu_item--;
        //ui->update_menu(ui);
    }else if (ui->active_menu_item == 0)
    {
        ui->active_menu_item = ui->item_count -1;
        //ui->update_menu(ui);
    }
    
}
void synth_ui_val_up(synth_ui_t *ui){
    if(*ui->menu_items[ui->active_menu_item].value < 255){
        (*ui->menu_items[ui->active_menu_item].value) ++;
    }
}
void synth_ui_val_down(synth_ui_t *ui){
    if(*ui->menu_items[ui->active_menu_item].value > 0){
        (*ui->menu_items[ui->active_menu_item].value) --;
    }
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
        lv_label_set_text_fmt(new_item->label,"%s    %d", new_item->name, (int)*new_item->value);
        lv_obj_set_style_text_color(new_item->label, lv_color_hex(synth_ui->text_color), 0);
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

