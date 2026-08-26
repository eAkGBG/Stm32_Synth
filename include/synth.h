#ifndef SYNTH_H
#define SYNTH_H

#include <stdint.h>

typedef struct synth_data_t {
    volatile int32_t attack;
    volatile int32_t decay;
    volatile int32_t sustain;
    volatile int32_t release;
    void (*set_attack)(struct synth_data_t *s, int32_t a);
} synth_data_t;

void synth_data_init(synth_data_t *synth);

#endif