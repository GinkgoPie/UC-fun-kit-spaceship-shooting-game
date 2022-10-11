/** @file   game.c
    @author Jiahe Bao
    @date   01 october 2022
    @brief  A shooting game to be played by two players with two different attacks.

    @defgroup
*/


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

//#include <string.h>

#define GAME_START_PACER_RATE 500
#define START_MESSAGE_RATE 10
#define SHIP_INIT_RATE 500
#define ledmat_rows 7
#define ledmat_cols 5
#define END 127

static uint8_t display[DISPLAY_WIDTH] =
{
    0x00, 0x00, 0x70, 0x00, 0x00
};

static uint8_t cockpit[2] = {4,2}; // Initially, The head location of our ship is at row 5 column 3;
static bool SHOOTING_STATUS = false;
static bool END = false;


static void game_start (void) {

    tinygl_init (GAME_START_PACER_RATE);
    tinygl_font_set (&font5x7_1);
    tinygl_text_speed_set (START_MESSAGE_RATE);
    tinygl_text_mode_set (TINYGL_TEXT_MODE_SCROLL);



    char display[] = "May the best man win\0";
    char *ptr1 = display;
    tinygl_text(ptr1);


    pacer_init (GAME_START_PACER_RATE);

    while (!navswitch_push_event_p(4))
    {
        pacer_wait();
        tinygl_update();
        navswitch_update();

    }

    navswitch_update();
}

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


static void update_ledmat (void)
{

    static uint8_t col = 0;

    ledmat_display_column (display[col], col);

    col++;
    if (col >= DISPLAY_WIDTH)
        col = 0;

}


static void game_end(void)
{

   ir_uart_putc(END);

}


static void IO_receive(void)
{
    uint8_t received_num = ir_uart_getc();
    if (received_num == cockpit[1]) {
        display_pixel_set(cockpit[1],cockpit[0],0);
        if (cockpit[0] < 6) {
            cockpit[0] ++;
        } else  {
            game_end(END); //I lose
        }
    } else if (received_num == END) {
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

   if (SEND_READY) {
      IO_send(cockpit[1]);
      SEND_READY = false;
   }
}


static void switch_status_check (void)
{
    navswitch_update();
    if (navswitch_push_event_p(NAVSWITCH_EAST)) {
        column_shift_right();
    } else if (navswitch_push_event_p(NAVSWITCH_WEST)) {
        column_shift_left();
    } else if (navswitch_push_event_p(NAVSWITCH_PUSH)) {

        SHOOTING_STATUS = true;
        SEND_READY = true;
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
          pattern1 = pattern1 | (1<<i);
      }
      ledmat_display_column (pattern2, colunm);
      SHOOTING_STATUS = false;
  } else {

  }
}



int main (void)
{

    system_init ();
    display_init();
    ir_uart_init ();
    pacer_init(800);

    game_start();

    uint8_t update_ledmat_tick = 0;
    uint8_t switch_status_check_tick = 0;
    uint8_t IO_check_tick = 0;
    uint8_t shooting_check_tick = 0;
    while(1)
    {
        pacer_wait();

        shooting_check_tick++;
        update_ledmat_tick++;
        switch_status_check_tick++;
        IO_check_tick++;
        if (shooting_check_tick >=10) {
            shooting_check_tick = 0;
            ship_shoot();
        }

        if (update_ledmat_tick >=3) {
            update_ledmat_tick = 0;
            update_ledmat();
        }

        if (switch_status_check_tick>=2) {
            switch_status_check_tick = 0;
            switch_status_check();
        }

        if (IO_check_tick>=20) {
            IO_check_tick = 0;
            IO_update();
        }




    }
    return 0;
}
