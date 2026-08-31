#ifndef UI_H
#define UI_H
#include "lvgl.h"
#include "synth.h"
#include "main.h"
//#include <stdio.h>
#include <stdlib.h>

//the font is definedt to use the 20 size in lv_conf.h
//I deactivated most stuff inside there. perhaps it's possible to deactivate more stuff.

#define UI_ROWS 11 //25pixel rows we can fit 11 of those on the display 280pixel display. 
#define UI_BUFFER 24 //After testing. 24 is the limit of chars that fit in 1 row of the 240pixel screen width with the current font.
#define LABEL_PADDING_LEFT 15 //Add some padding 10pixels about one char.
#define LABEL_PADDING_TOP 25 //my display has rounded corners. lets simply start the menu 1 line down so we acually only can diplay 9 ui rows.
#define LABEL_ROW_SIZE 25 //set the height of each row 25 is good for 20 size text
#define TEXT_PADDING_LEFT 10 //add some padding for text inside labels
#define TEXT_PADDING_RIGHT 25
typedef struct synth_ui_item_t{
    //Here we create the menu item data type.
    //What does it need? it needs Label. And a value. how does it connect to the value?
    //Can think about that later.
    char *name; //The display is 24 chars. so i figure 20/4 = 5 we can keep 2 char space on each side.
    //that leavs 15 chars for the label and 5 to display the value.
    volatile int32_t *value; //I decided that I will connect directly to the variable inside the synth struct.
    lv_obj_t *label;
    bool active;

}synth_ui_item_t;

typedef struct synth_ui_t{
    //How to build this struct? the screen will fit 11 rows. so it will need
    //A screen struct.
    lv_obj_t *screen;
    //A beckground color.
    uint32_t bg_color;
    //A text color
    uint32_t text_color;
    //A selected color I think to make this simple we make a box that we can move around with a lighter color
    uint32_t active_color;
    //under the text obj
    uint32_t active_menu_item;
    //How to build the menu an array of menu pages. or should we simply let it scroll up and down?
    //for starters i think we simply make an array of menu items these probably be a item struct
    synth_ui_item_t *menu_items;
    //for a pointer we need to keep track how manny items there are.
    uint32_t item_count;
}synth_ui_t;

//void synth_ui_init(void);
void synth_ui_init(synth_ui_t *synth_ui, synth_data_t *synth);
void synth_ui_test_print(int32_t value);
void synth_ui_add_item(synth_ui_t *synth_ui, char *name, volatile int32_t *value, bool active);
#endif
