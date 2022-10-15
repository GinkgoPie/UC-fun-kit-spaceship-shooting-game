#ifndef SHIP_H
#define SHIP_H 

#include "system.h"
#include "ledmat.h"
#include "pacer.h"
#include "led.h"
#include "pio.h"
#include "timer.h"
#include "tinygl.h"
#include "display.h"
#include "../fonts/font5x7_1.h"
#include "task.h"
#include "navswitch.h"
#include "delay.h"
#include "ir_uart.h"
#include <stdio.h>

typedef struct {
    uint8_t row;
    uint8_t col;
} Cockpit_t;

static void column_shift_right(void);

static void column_shift_left(void);

static void ship_shoot(void);


#endif
