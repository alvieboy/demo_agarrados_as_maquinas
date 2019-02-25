#include "effect.h"
#include "effect_classic.h"
#include "effect_decay.h"
#include "effect_hsv.h"
#include "effect_move1.h"
#include "effect_move2.h"
#include <stdlib.h>
#include "setup.h"

#define EFFECT_WORK_AREA_SIZE 4096
static uint8_t workarea[EFFECT_WORK_AREA_SIZE];

static const effect_classic_t classic_blue  = { 0x0000FF };
static const effect_classic_t classic_red   = { 0x00FF00 };
static const effect_classic_t classic_green = { 0xFF0000 };

static const effect_decay_t   decay_blue    = { 0x00007F };
static const effect_decay_t   decay_red     = { 0x007F00 };
static const effect_decay_t   decay_green   = { 0x7F0000 };

static const effect_hsv_t     hsv1   = { 0x7F0000 };

const struct effect_t effects[] = {
    { "Classico", "Cor azul", &effect_classic_init, &effect_classic_activate, &effect_classic_tick, &classic_blue },
    { "Classico", "Cor verde", &effect_classic_init, &effect_classic_activate, &effect_classic_tick, &classic_green },
    { "Classico", "Cor vermelha", &effect_classic_init, &effect_classic_activate, &effect_classic_tick, &classic_red },
    { "Decay", "Cor azul", &effect_decay_init, &effect_decay_activate, &effect_decay_tick, &decay_blue },
    { "Decay", "Cor verde", &effect_decay_init, &effect_decay_activate, &effect_decay_tick, &decay_green },
    { "Decay", "Cor vermelho", &effect_decay_init, &effect_decay_activate, &effect_decay_tick, &decay_red },
    { "HSV", "Fogo", &effect_hsv_init, &effect_hsv_activate, &effect_hsv_tick, &hsv1 },
    { "Animado", "Metade zig-zag", &effect_move1_init, &effect_move1_activate, &effect_move1_tick, NULL },
    { "Animado", "Total zig-zag", &effect_move2_init, &effect_move2_activate, &effect_move2_tick, NULL },
};

//static effect_t *current_effect = &effects[0];
static int current_effect_index = 0;

#define NUM_EFFECTS (sizeof(effects)/sizeof(effects[0]))

const effect_t *effect(void) {
    return &effects[current_effect_index];
}

void effect__init(void)
{
    effect()->init(effect(),workarea);
}

void effect__next(void)
{
    if (current_effect_index==0) {
        current_effect_index = NUM_EFFECTS-1;
    } else {
        current_effect_index--;
    }
    effect()->init(effect(), workarea);
}

void effect__previous(void)
{
    if (current_effect_index==NUM_EFFECTS-1) {
        current_effect_index = 0;
    } else {
        current_effect_index++;
    }
    effect()->init(effect(), workarea);
}

void effect__tick(void)
{
    effect()->tick(effect(), workarea);
}

void effect__activate(void)
{
    effect()->activate(effect(), workarea);
}


