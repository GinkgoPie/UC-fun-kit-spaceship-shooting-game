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

static void column_shift_right(void) {
    uint8_t *copy = NULL;
    *copy = display[LEDMAT_COLS_NUM-1];
    display[4] = display[3];
    display[3] = display[2];
    display[2] = display[1];
    display[1] = display[0];
    display[0] = *copy;
    if (cockpit[1] == 4) {
        cockpit[1] = 0;
    } else {
        cockpit[1] = (cockpit[1] + 1);
    }
}

static void column_shift_left(void) {
    uint8_t *copy = NULL;
    *copy = display[0];
    display[0] = display[1];
    display[1] = display[2];
    display[2] = display[3];
    display[3] = display[4];
    display[4] = *copy;
    if (cockpit[1] == 0) {
        cockpit[1] = 4;
    } else {
        cockpit[1] = (cockpit[1] - 1)   ;
    }

}

static void ship_shoot(void)
{
  uint8_t colunm = cockpit[1];
  uint8_t pattern1 = display[colunm]|(1<<(7-cockpit[0]-1));
  uint8_t pattern2 = display[colunm]|(1);
  if (SHOOTING_STATUS){
      for (uint8_t i =cockpit[0]; i>0; i--) {
          ledmat_display_column (pattern1, colunm);
          pacer_wait();
          pattern1 |= (1<<i);
      }
      ledmat_display_column (pattern2, colunm);
      SHOOTING_STATUS = false;
  } else {

  }
}
