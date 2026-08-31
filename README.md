STM32 Synth.
This project is a test to create a synth with the stm32f401cc (Black Pill board)
Iused LLM to help write the hardware setup code, instead of CubeMX.
Ok there were issues with the dac. so now the dac is connected to an external board.
With spi, and it sends 128 sampels with interrupt. Ordered a better stm32f411 black pill.
Hope llm were not lying it was supposed to be able to set i2s clocks specifically for dac.
will see when it arives if not atleast i hawe a stm32 with higher performance for the synth.

Goal of the project.
    Create a simple Synth that uses the pcm5100a Dac module i recived in a kit.
    Use the 240x280 lcd display i got from amazon with the ST7789V2 controller chip
    Use LVGL library to create a simple GUI for the synth.
    For now there are 4 buttons on the board.
        I figured 2 buttons can be menu navigation forward and back,
        pressing back wil select previous menu item and forward the next.
        pressing both at the same time will menu dive, example main menu has different settings, option 1 adsr, option 2 oscilator. pressing both will enter that sub menu.
        inside the sub menu there will be a back item pressing both when selectid will go back. navigating vill always set that item active. (we van also make it so that the + and - value buttons acts as enter when a menu diving item is selected. perhaps the best)

        2 buttons i call these upp and down. they are used to increase/decrease the value in the selected menu item.

    Keyboard
        i'm considering buying a multizone tof sensor for this. hold your hand in the air and it will play notes. also i can simply use a 1 beam tof sensor then note is simply the hight above the sensor. This is not decided yet.

ToDo:
    The hardware init code is running. Clean it up move functions to apropriated C files to clean main up, alredy started this.
    
    Buld a struct that holds the synth data.
        Connect the synth struct inside the buffer fill interrupt.
        Make it keep track of time in some way. we use 128 sampels buffer length and 48khz
        So we can probably use this in some way for keeping time.

    Build a simple ADSR function and a wave lookup table to send out on the DAC

    Buld the menu. Start with only ADSR settings
        Started with it. it now stores data nicely in 2 structs hooked to the synth struct,
        can change values inside the synth when functions added.
            I tried function pointers inside structs. Will use this.

    Come up with more stuff to add / do

    Fix this readme? Looks strange on github, not line breaking where I pressed enter. Look this up.