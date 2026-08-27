#ifndef SYNTH_H
#define SYNTH_H

#include <stdint.h>

//This struct are supposed to hold the synth data and function pointers to synth operations.
typedef struct synth_data_t {
    volatile int32_t attack;
    volatile int32_t decay;
    volatile int32_t sustain;
    volatile int32_t release;
    void (*set_attack)(struct synth_data_t *synth, int32_t value);
    int32_t (*get_attack)(struct synth_data_t *synth);
    void (*set_decay)(struct synth_data_t *synth, int32_t value);
    int32_t (*get_decay)(struct synth_data_t *synth);
    void (*set_sustain)(struct synth_data_t *synth, int32_t value);
    int32_t (*get_sustain)(struct synth_data_t *synth);
    void (*set_release)(struct synth_data_t *synth, int32_t value);
    int32_t (*get_release)(struct synth_data_t *synth);
} synth_data_t;

void synth_data_init(synth_data_t *synth);

#endif