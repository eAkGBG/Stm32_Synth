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
    synth->sustain = 200;
    synth->release = 10;
    
    synth->set_attack = synth_set_attack;
    synth->get_attack = synth_get_attack;

    synth->set_decay = synth_set_decay;
    synth->get_decay = synth_get_decay;

    synth->set_sustain = synth_set_sustain;
    synth->get_sustain = synth_get_sustain;

    synth->set_release = synth_set_release;
    synth->get_release = synth_get_release;

}