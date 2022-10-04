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

static uint8_t bitmap_led[] =
{
    0x00, 0x00, 0x70, 0x00, 0x00
};

//static uint8_t bitmap_ship_init = 0x70;

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


}

static void column_shift_right(void) {
    uint8_t *copy = NULL;
    *copy = bitmap_led[LEDMAT_COLS_NUM-1];
    bitmap_led[4] = bitmap_led[3];
    bitmap_led[3] = bitmap_led[2];
    bitmap_led[2] = bitmap_led[1];
    bitmap_led[1] = bitmap_led[0];
    bitmap_led[0] = *copy;

}

static void column_shift_left(void) {
    uint8_t *copy = NULL;
    *copy = bitmap_led[0];
    bitmap_led[0] = bitmap_led[1];
    bitmap_led[1] = bitmap_led[2];
    bitmap_led[2] = bitmap_led[3];
    bitmap_led[3] = bitmap_led[4];
    bitmap_led[4] = *copy;
}


static void display_task (__unused__ void *data)
{

    static uint8_t current_column = 0;
    game_start();
    while (1)
    {

        pacer_wait ();

        ledmat_display_column (bitmap_led[current_column], current_column);

        current_column++;

        if (current_column > (LEDMAT_COLS_NUM - 1))
        {
            current_column = 0;
        }

        navswitch_update();
        if (navswitch_push_event_p(1)) {
            column_shift_right();
            ledmat_display_column (bitmap_led[current_column], current_column);
            current_column++;
        }

        if (navswitch_push_event_p(3)) {
            column_shift_left();
            ledmat_display_column (bitmap_led[current_column], current_column);
            current_column++;
        }

    }

}

int main (void)
{





    task_t tasks[] =
    {
        {.func = display_task, .period = TASK_RATE / SHIP_INIT_RATE,
         .data = 0}

    };


    system_init ();
    game_start();
    ledmat_init();

    task_schedule (tasks, ARRAY_SIZE (tasks));
    return 0;

}
