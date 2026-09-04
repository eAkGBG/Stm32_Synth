//Human code By Me
#include "synth.h"

//Lets write the note selection function.
void synth_select_note(synth_data_t *synth){
    /*
    NOTE
FREQUENCY (HZ)
C2	
65.41
C#2 / Db2	
69.30
D2	
73.42
D#2 / Eb2	
77.78
E2	
82.41
F2	
87.31
F#2 / Gb2	
92.50
G2	
98.00
G#2 / Ab2	
103.83
A2	
110.00
A#2 / Bb2	
116.54
B2	
123.47
C3
130,82
*/
    if( synth->tof_distance < 261 && synth->tof_status == 0){
        synth->master_amp = 0.01f;
        synth->note_on = true;
        float_t current_note = (synth->tof_distance / 2) / 12;
        if(current_note < 1){
            synth->note = 65;
        }else if(current_note < 2){
            synth->note = 69;
        }else if(current_note < 3){
            synth->note = 73;
        }else if(current_note < 4){
            synth->note = 78;
        }else if(current_note < 5){
            synth->note = 82;
        }else if(current_note < 6){
            synth->note = 87;
        }else if(current_note < 7){
            synth->note = 93;
        }else if(current_note < 8){
            synth->note = 98;
        }else if(current_note < 9){
            synth->note = 104;
        }else if(current_note < 10){
            synth->note = 110;
        }else if(current_note < 11){
            synth->note = 117;
        }else if(current_note < 12){
            synth->note = 123;
        }
    } else {
        synth->master_amp = 0.0001f;
        synth->note_on = false;
    }

}

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
    synth->attack = 100;
    synth->decay = 0;
    synth->sustain = 200;
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

    synth->set_master_amp = synth_master_volume;

}

//We need to create an ADSR function i think what is needed ? calculate a step value to scale the current sample amplitude. and hook that to the oscilator function.
//but instead of using the note hz value it needs to scale over time. there are probably many tutorials on this on the internet.
void synth_osc1_adsr(synth_data_t *synth){
    //ToDo fix the bugs. ceck for division by 0 errors, ceck for typos, fix type conversions. and more. check the if statements so that they acually are only running when the ticks are correct.
    uint16_t lr_inc = 0;
    float_t atk_amp = 0.000001;
    float_t dec_amp = 0.000001;
    float_t rel_amp = 0.000001;
    int16_t temp_amp; //used as a temporary variaqble for converting between types for calculations.
    //sustain this value is only a amp value. figure out how to create this value it's a scale so 1/ ((65536/256) * sustain) ?
    float_t sus_amp = ((float_t)synth->sustain)/256.0f;

    uint32_t atk_time_steps = ((48000/1000)*((synth->attack) * 10));    //I need to calculate the steps needed to reach the attack * 10 (i decided we go in 10ms steps)
    uint32_t atk_step_size = 65536 / atk_time_steps;                    //I need to calculate the step size.

    //do the same for decay.
    uint32_t dec_time_steps = ((48000/1000)*((synth->decay) * 10));
    uint32_t dec_step_size = 65536 / dec_time_steps;
    
    //do some logic if note on(we are later going to send a note on true/false into the synth stryct now it's allways on.)
    synth->note_on = true;
    if(synth->ticks < atk_time_steps){
        synth->af = false;
    }
    if(synth->ticks < atk_time_steps + dec_time_steps){
        synth->df = false;
    }
    //do the attack scaling value.
    if(!synth->af){
        lr_inc = 0;
        for(int i = 0; i < BUFFER_SIZE; i++){
            atk_amp = (float_t)synth->acc_atk_time/65535.0f; //devide 65536 by the accumulator to get the scalar value.
            temp_amp = (int16_t)(synth->buffer[lr_inc]  - 32768);// - 32768;
            temp_amp = temp_amp * atk_amp;
            (synth->buffer[lr_inc++]) = (uint16_t)(temp_amp);
            (synth->buffer[lr_inc++]) = (uint16_t)(temp_amp);
            synth->acc_atk_time += atk_step_size;
            (synth->ticks)++; //Remember we need to put pointers inside () or else C does stupid stuff with adresses
        }
        if(synth->ticks >= atk_time_steps){
            synth->af = true;
            //synth->ticks = 0;
        }

    }
    //do the decay scaling value.
    //ToDo: we hawe to count in reverse here.!
    if(synth->af && !synth->df){
        lr_inc = 0;
        for(int i = 0; i < BUFFER_SIZE; i++){
            dec_amp = (float_t)(synth->acc_dec_time)/65535.0f; //devide 65536 by the accumulator to get the scalar value.
            temp_amp = (int16_t)(synth->buffer[lr_inc] - 32768);
            temp_amp = temp_amp * dec_amp;
            (synth->buffer[lr_inc++]) = ((uint16_t)temp_amp);
            (synth->buffer[lr_inc++]) = ((uint16_t)temp_amp);
            synth->acc_dec_time += dec_step_size;
            (synth->ticks)++; //Remember we need to put pointers inside () or else C does stupid stuff with adresses
        }
        if(synth->ticks >= dec_time_steps + atk_time_steps){
            synth->df = true;
            //synth->ticks = 0;
        }

    }
     //lets test sustain and reset tics.
    if (synth->af && synth->df && synth->note_on)
    {
        lr_inc = 0;
        
        for(int i = 0; i < BUFFER_SIZE; i++){
            temp_amp = (float_t)(synth->buffer[lr_inc] - 32768);
            temp_amp = temp_amp * sus_amp;
            uint16_t converted_sample = (uint16_t)temp_amp;
            (synth->buffer[lr_inc++]) = converted_sample;
            (synth->buffer[lr_inc++]) = converted_sample;
        }
    }
}    
void synth_master_volume(synth_data_t *synth){
    int16_t temp_amp = 0;
    int16_t lr_inc = 0;
    uint16_t converted_sample = 0;
    for(int i = 0; i < BUFFER_SIZE; i++){
            //Left Channel
            temp_amp = (float_t)(synth->buffer[lr_inc]);
            temp_amp = temp_amp * (synth->master_amp);
            converted_sample = (uint16_t)temp_amp;
            (synth->buffer[lr_inc++]) = converted_sample;
            //Right channel
            temp_amp = (float_t)(synth->buffer[lr_inc]);
            temp_amp = temp_amp * (synth->master_amp);
            converted_sample = (uint16_t)temp_amp;
            (synth->buffer[lr_inc++]) = converted_sample;
        }
}

//this function fills the buffer with oscilator waveform
void synth_osc1_generator(synth_data_t *synth){
    uint16_t lr_inc = 0;
    
    synth_select_note(synth);

    synth->step = 65536*(synth->note)/SAMPLE_RATE;
    for(int i = 0; i < BUFFER_SIZE; i++){
        synth->buffer[lr_inc++] = synth->phase;
        synth->buffer[lr_inc++] = synth->phase;
        synth->phase += synth->step;    
    }
    synth->osc1_adsr(synth);
    synth->set_master_amp(synth); //this function comes last.
}

//ToDo add a master volume function.