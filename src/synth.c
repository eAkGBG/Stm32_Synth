#include "synth.h"

//Helper functions to get and set ADSR
void synth_set_attack(synth_data_t *synth, int32_t attack){
    synth->attack = attack;
}
int32_t synth_get_attack(synth_data_t *synth){
    return synth->attack;
}

void synth_set_decay(synth_data_t *synth, int32_t decay){
    synth->decay = decay;
}
int32_t synth_get_decay(synth_data_t *synth){
    return synth->decay;
}

void synth_set_sustain(synth_data_t *synth, int32_t sustain){
    synth->sustain = sustain;
}
int32_t synth_get_sustain(synth_data_t *synth){
    return synth->sustain;
}

void synth_set_release(synth_data_t *synth, int32_t release){
    synth->release = release;
}
int32_t synth_get_release(synth_data_t *synth){
    return synth->release;
}

void synth_data_init(synth_data_t *synth){
    synth->attack = 5;
    synth->decay = 15;
    synth->sustain = 20;
    synth->release = 10;
    synth->master_amp = MASTER_AMP;
    synth->note = 440; //init with A
    synth->phase = 0;
    synth->step = 65536*(synth->note)/SAMPLE_RATE; //Need to solve how to handle notes. A lookup table?
    synth->ticks = 0; //total time for our tics in 48khz will last for 89 478 seconds. enough to never run out of time steps.
    synth->note_on = false;
    
    synth->set_attack = synth_set_attack;
    synth->get_attack = synth_get_attack;

    synth->set_decay = synth_set_decay;
    synth->get_decay = synth_get_decay;

    synth->set_sustain = synth_set_sustain;
    synth->get_sustain = synth_get_sustain;

    synth->set_release = synth_set_release;
    synth->get_release = synth_get_release;

    synth->osc1_adsr = synth_osc1_adsr;
    synth->osc1_generator = synth_osc1_generator;

}

//We need to create an ADSR function i think what is needed ? calculate a step value to scale the current sample amplitude. and hook that to the oscilator function.
//but instead of using the note hz value it needs to scale over time. there are probably many tutorials on this on the internet.
void synth_osc1_adsr(synth_data_t *synth){
    //ToDo fix the bugs. ceck for division by 0 errors, ceck for typos, fix type conversions. and more.
    uint16_t lr_inc = 0;
    float_t atk_amp = 0;
    float_t dec_amp = 0;
    float_t rel_amp = 0;
    //sustain this value is only a amp value. figure out how to create this value it's a scale so 1/ ((65536/256) * sustain) ?
    float_t sus_amp = 1/((65536/256)*(synth->sustain));

    uint32_t atk_time_steps = ((48000/1000)*((synth->attack) * 10));    //I need to calculate the steps needed to reach the attack * 10 (i decided we go in 10ms steps)
    uint32_t atk_step_size = 65536 / atk_time_steps;                    //I need to calculate the step size.

    //do the same for decay.
    uint32_t dec_time_steps = ((48000/1000)*((synth->decay) * 10));
    uint32_t dec_step_size = 65536 / dec_time_steps;
    
    
    //do the attack scaling value.
    if(synth->ticks < atk_time_steps){
        lr_inc = 0;
        synth->acc_atk_time += atk_step_size;
        atk_amp = 65536/(synth->acc_atk_time); //devide 65536 by the accumulator to get the scalar value.
        for(int i = 0; i < BUFFER_SIZE; i++){
            uint16_t temp_amp = ((uint16_t)synth->buffer[lr_inc]) * atk_amp;
            synth->buffer[lr_inc++] = temp_amp;
            synth->buffer[lr_inc++] = temp_amp;
            (synth->ticks)++; //Remember we need to put pointers inside () or else C does stupid stuff with adresses
        }

    }
    //do the decay scaling value.
    if(synth->ticks > atk_time_steps && synth->ticks < (atk_time_steps + dec_time_steps)){
        synth->acc_dec_time += dec_step_size;
        lr_inc = 0;
        dec_amp = 65536/(synth->acc_dec_time); //devide 65536 by the accumulator to get the scalar value.
        for(int i = 0; i < BUFFER_SIZE; i++){
            uint16_t temp_amp = ((uint16_t)synth->buffer[lr_inc]) * dec_amp;
            synth->buffer[lr_inc++] = temp_amp;
            synth->buffer[lr_inc++] = temp_amp;
            (synth->ticks)++; //Remember we need to put pointers inside () or else C does stupid stuff with adresses
        }

    }
    //lets test sustain and reset tics.
    if (synth->ticks > (atk_time_steps + dec_time_steps))
    {
        lr_inc = 0;
        for(int i = 0; i < BUFFER_SIZE; i++){
            synth->buffer[lr_inc++] *= sus_amp;
            synth->buffer[lr_inc++] *= sus_amp;
        }
    }
    
    

    
}
//this function fills the buffer with oscilator waveform
void synth_osc1_generator(synth_data_t *synth){
    uint16_t lr_inc = 0;

    for(int i = 0; i < BUFFER_SIZE; i++){
        synth->buffer[lr_inc++] = synth->phase;
        synth->buffer[lr_inc++] = synth->phase;
        synth->phase += synth->step;    
    }
    synth->osc1_adsr(synth);
    //synth->osc1_master(synth); //this function comes last.
}

//ToDo add a master volume function.