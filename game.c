/** @file   game.c
    @author Jiahe Bao
    @date   01 october 2022
    @brief  A shooting game to be played by two players with two different attacks.

    @defgroup
*/


#include "system.h"
#include "pacer.h"
#include "pio.h"
#include "timer.h"
#include "tinygl.h"
#include "../fonts/font5x7_1.h"
#include "task.h"
#include "navswitch.h"
#include "delay.h"
#include "ir_uart.h"
#include <stdio.h>
#include "ship.h"
#include "game_display.h"
//#include <string.h>

#define GAME_START_PACER_RATE 500
#define START_MESSAGE_RATE 10
#define SHIP_INIT_RATE 500
#define LEDMAT_ROWS 6
#define LEDMAT_COLS 4
#define END_MAGIC 127

uint8_t display[] =
{
    0x00, 0x00, 0x70, 0x00, 0x00
};

uint8_t *displayPtr = &display;


static ship_t myShip;
static ship_t* myShipPtr;
static bool SHOOTING_STATUS = false;
static bool SEND = false;
static bool RECEIVED = false;
static bool WON = false;
static bool LOSE = false;
static uint8_t bullet_col = 0;
static uint8_t bullet_row = 0;
static uint8_t damage_col = 0;
static uint8_t damage_row = 0;


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
    display_clear();
    navswitch_update();
}






static void IO_receive(void)
{
    uint8_t received_num = ir_uart_getc();
    damage_col = received_num;
    if (received_num == myShipPtr->head_column) {

        single_pixel_set(displayPtr,myShipPtr->head_column,myShipPtr->head_row,0);

        if (myShipPtr->head_row < LEDMAT_ROWS) {
            myShipPtr->head_row ++;
        } else{
            LOSE = true;
            ir_uart_putc(END_MAGIC); //I lose
        }
    } else if (received_num == END_MAGIC) {
        WON = true;


    }


}



static void IO_send(uint8_t col)
{

    uint8_t inversed_col = LEDMAT_COLS-col;
    ir_uart_putc(inversed_col);

}


static void IO_update(void)
{
   if (ir_uart_read_ready_p()) {
       RECEIVED = true;
       IO_receive();

   }

   if (SEND) {
      IO_send(myShipPtr->head_column);
      SEND = false;
   }
}


static void switch_status_check (void)
{
    navswitch_update();
    if (navswitch_push_event_p(NAVSWITCH_EAST)) {
        column_shift_right(displayPtr);
        ship_to_right(myShipPtr);
    } else if (navswitch_push_event_p(NAVSWITCH_WEST)) {
        column_shift_left(displayPtr);
        ship_to_left(myShipPtr);
    } else if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
        bullet_col = (myShipPtr->head_column);
        bullet_row = (myShipPtr->head_row)-2;
        SHOOTING_STATUS = true;
        SEND = true;
    }


}



static void bullet_shoot(void)
{
    // Turning off the led from previous bullet row, turn on the next intended led;
    if (SHOOTING_STATUS && bullet_row != 0){
        single_pixel_set(displayPtr,bullet_col,bullet_row+1 ,0);
        single_pixel_set(displayPtr,bullet_col,bullet_row ,1);
        bullet_row--;
    }

    //Change the status back to not shooting when bullet reaches to row 0;
    else if (SHOOTING_STATUS && bullet_row == 0) {
        single_pixel_set(displayPtr,bullet_col,1,0);
        single_pixel_set(displayPtr,bullet_col,0,1);
        SHOOTING_STATUS = false;
    }

    // while not shooting, turn off row 0 led;
    else if (!SHOOTING_STATUS) {
        single_pixel_set(displayPtr,bullet_col,0,0);
    }

}

static void bullet_receive(void)
{
    uint8_t bullet_end_row = LEDMAT_ROWS;

    if (damage_col == myShipPtr->head_column) {
        bullet_end_row = myShipPtr->head_row;
    }

    if (RECEIVED && damage_row == 0) {
        single_pixel_set(displayPtr,damage_col,0,1);
        damage_row++;
    }

    else if (RECEIVED && damage_row < bullet_end_row ){
        single_pixel_set(displayPtr,damage_col,damage_row-1,0);
        single_pixel_set(displayPtr,damage_col,damage_row,1);
        damage_row++;
    }

    else if (RECEIVED && damage_row == bullet_end_row) {
        single_pixel_set(displayPtr,damage_col,damage_row-1,0);
        damage_row =0;
        RECEIVED = false;
    }


}

static void end_display(void)
{


    tinygl_init (GAME_START_PACER_RATE);
    tinygl_font_set (&font5x7_1);
    tinygl_text_speed_set (START_MESSAGE_RATE);
    tinygl_text_mode_set (TINYGL_TEXT_MODE_SCROLL);

    char display[] = "END\0";
    char *ptr1 =display ;

    tinygl_text(ptr1);
    pacer_init(500);
    while (1)
    {
        pacer_wait();
        tinygl_update();
        navswitch_update();
    }

}



int main (void)
{

    system_init ();
    display_init();
    ir_uart_init ();
    pacer_init(200);
    myShip = ship_init();
    myShipPtr = &myShip;
    game_start();


    uint8_t update_ledmat_tick = 0;
    uint8_t switch_status_check_tick = 0;
    uint8_t IO_check_tick = 0;
    uint8_t bullet_display_tick = 0;
    while(1)
    {
        pacer_wait();

        bullet_display_tick++;
        update_ledmat_tick++;
        switch_status_check_tick++;
        IO_check_tick++;

        if (bullet_display_tick >=10) {
            bullet_display_tick = 0;
            bullet_shoot();
            bullet_receive();

        }


        if (update_ledmat_tick >=3) {
            update_ledmat_tick = 0;
            update_ledmat(&display);
        }

        if (switch_status_check_tick>=2) {
            switch_status_check_tick = 0;
            switch_status_check();
        }

        if (IO_check_tick>=20) {
            IO_check_tick = 0;
            IO_update();
        }

        if (WON||LOSE) {
            break;
        }



    }
    end_display();
    return 0;
}
