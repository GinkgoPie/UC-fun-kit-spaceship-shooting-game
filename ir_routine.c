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

#include "ir_routine.h"
#include "ship.h"

#include <stdio.h>

static void IO_receive(void)
{
    uint8_t received_num = ir_uart_getc();
    if (received_num == 4) {
      led_init ();
      led_set (LED1, 1);
    }
    if (received_num == cockpit[1]) {
        display_pixel_set(cockpit[1],cockpit[0],0);

        if (cockpit[0] < 6) {
            cockpit[0] ++;
        } else  {
            game_end(); //I lose
        }
    } else if (received_num == END_MAGIC) {
        //I won

    }

}


static void IO_send(uint8_t col)
{

    uint8_t inversed_col = ledmat_cols-col;
    ir_uart_putc(inversed_col);

}


static void IO_update(void)
{
   if (ir_uart_read_ready_p()) {
       IO_receive();
   }

   if (SEND) {
      IO_send(cockpit[1]);
      SEND = false;
   }
}
