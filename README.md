STM32 Synth.
This project is a test to create a synth with the stm32f401cc (Black Pill board)
Iused LLM to help write the hardware setup code, instead of CubeMX.
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

    Build a simple ADSR function and a wave lookup table to send out on the DAC

    Buld the menu. Start with only ADSR settings

    Come up with more stuff to add / do