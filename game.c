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
#include <stdio.h>
//#include <string.h>

#define GAME_START_PACER_RATE 500
#define START_MESSAGE_RATE 10
#define SHIP_INIT_RATE 500

static uint8_t display[DISPLAY_WIDTH] =
{
    0x00, 0x00, 0x70, 0x00, 0x00
};



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

}

static void column_shift_left(void) {
    uint8_t *copy = NULL;
    *copy = display[0];
    display[0] = display[1];
    display[1] = display[2];
    display[2] = display[3];
    display[3] = display[4];
    display[4] = *copy;
}


static void update_ledmat (void)
{

    static uint8_t col = 0;

    ledmat_display_column (display[col], col);

    col++;
    if (col >= DISPLAY_WIDTH)
        col = 0;

}

static void switch_status_check (void)
{
    navswitch_update();
    if (navswitch_push_event_p(NAVSWITCH_EAST)) {
        column_shift_right();
    } else if (navswitch_push_event_p(NAVSWITCH_WEST)) {
        column_shift_left();
    } else if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
        uint8_t current_row = ;
        uint8_t current_col = ;
        ship_shoot(current_row,current_col);
        IO_send(current_row);
    }


}


static void ship_shoot(uint8_t row , uint8_t col)
{

}

static void IO_update()
{
    
}

uint8_t inversed_col void IO_receive(void)
{


}


static void IO_send(uint8_t inversed_col)
{


}

int main (void)
{

    system_init ();
    display_init();
    pacer_init(1000);

    game_start();

    uint8_t update_ledmat_tick = 0;
    uint8_t switch_status_check_tick = 0;
    uint8_t IO_check_tick = 0;
    while(1)
    {
        pacer_wait();

        update_ledmat_tick++;
        switch_status_check_tick++;
        IO_check_tick++;

        if (update_ledmat_tick >=2) {
            update_ledmat_tick = 0;
            update_ledmat();

        if (switch_status_check_tick>=5) {
            switch_status_check_tick = 0;
            switch_status_check();
        }

        if (IO_check_tick>=5) {
            IO_check_tick = 0;
            IO_update();
        }

        }
    }
    return 0;
}
