#ifndef IR_ROUTINE_H
#define IR_ROUTINE_H

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

#include "ship.h"

#include <stdio.h>

static void IO_receive(void);

static void IO_send(uint8_t col);

static void IO_update(void)

#endif
