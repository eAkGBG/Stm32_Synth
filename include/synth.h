#ifndef SYNTH_H
#define SYNTH_H

#include <stdint.h>
#include <math.h>
#include <stdbool.h>
//#include <stdio.h>
#include "main.h"


//Some defines to controll it all.
#define BUFFER_SIZE 128 //The synth are working with a buffer size of 128 samples. it calculates and send them over to the sound device
#define SAMPLE_RATE 48000 //Synth are running at 16bit 48khz.
#define MASTER_AMP 0.02 //this one keeps a nice max level.

//This struct are supposed to hold the synth data and function pointers to synth operations.
typedef struct synth_data_t {
    volatile int32_t attack;
    volatile int32_t decay;
    volatile int32_t sustain;
    volatile int32_t release;
    float_t master_amp;
    uint32_t acc_atk_time;
    uint32_t acc_dec_time;
    uint32_t acc_rel_time;

    //keep the status if we finished the ADSR parts.
    bool af; 
    bool df;
    bool rf;

    volatile uint16_t phase; //this is used to generate the Wave form
    uint32_t step; //this is used to calculate the valu change for each sample from the phase
    uint32_t ticks; //keeps track of time wonder how manny i need this is how big the number is. 4 294 967 295/48000 = 89 478 seconds. that is more then enough..
    uint16_t buffer[BUFFER_SIZE*2]; // for stereo need to alternate L/R values so buffer to sound device needs to be x2 BUFFER_SIZE

    uint16_t tof_distance; //here we store the current distance from the sensor to calculate note values.
    bool note_on; //this one keeps track if the note is playing.
    uint16_t note; //keep the current note in memory. perhaps this needs to be an array for polyphony.. at least we want the release/decay of previous note to ring out.
    //berhaps sulution is to build an array of voices inside the synth struct. each time a new note is pressed it swaps to the next to let the old one ring out.
    //but mono synths the simply break the old note or glides over. for now simply let it break the old note.
    

    void (*set_attack)(struct synth_data_t *synth, int32_t value);
    int32_t (*get_attack)(struct synth_data_t *synth);
    void (*set_decay)(struct synth_data_t *synth, int32_t value);
    int32_t (*get_decay)(struct synth_data_t *synth);
    void (*set_sustain)(struct synth_data_t *synth, int32_t value);
    int32_t (*get_sustain)(struct synth_data_t *synth);
    void (*set_release)(struct synth_data_t *synth, int32_t value);
    int32_t (*get_release)(struct synth_data_t *synth);
    void (*osc1_adsr)(struct synth_data_t *synth);
    void (*osc1_generator)(struct synth_data_t *synth);
    void (*set_master_amp)(struct synth_data_t *synth);
} synth_data_t;
//to start i think perhaps we go with 10ms steps for the adsr.
void synth_osc1_adsr(synth_data_t *synth);
void synth_osc1_generator(synth_data_t *synth);
void synth_data_init(synth_data_t *synth);
void synth_master_volume(synth_data_t *synth);
#endif