/** @file   game.c
    @author Jiahe Bao, Callum Whitehead
    @date   01 October 2022
    @brief  A ship game to be played by two players with the objective 
            being to be the first to shoot the opponents ship 3 times.

    @defgroup
*/

//Header functions used by this 'game.c' program
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

//Number of initial values
#define GAME_START_PACER_RATE 500
#define START_MESSAGE_RATE 10
#define SHIP_INIT_RATE 500
#define LEDMAT_ROWS 6
#define LEDMAT_COLS 4
#define WINNER_CHECK 127
#define GAME_OVER_CHECK 6

//Intiatilise a bitmap array to display the ship. The array represents the ship being in the 3rd column and the hex value represents rows 5,6,7
uint8_t display[] =
{
    0x00, 0x00, 0x70, 0x00, 0x00
};



//Ships initialisation variables
static ship_t myShip;
static ship_t* myShipPtr;
static bool SHOOTING_STATUS = false;
static bool SEND = false;
static bool RECEIVED = false;
static bool WON = false;
static bool LOST = false;
static uint8_t bullet_col = 0;
static uint8_t bullet_row = 0;
static uint8_t damage_col = 0;
static uint8_t damage_row = 0x01;


static void game_start (void) {
/* This is the games' start function. It displays a splash screen that shows a scrolling message 
and waits for the player to press the nav switch in. */
    
    //Sets the font, speed, and mode of the text being displayed
    tinygl_init (GAME_START_PACER_RATE);
    tinygl_font_set (&font5x7_1);
    tinygl_text_speed_set (START_MESSAGE_RATE);
    tinygl_text_mode_set (TINYGL_TEXT_MODE_SCROLL);

    char start_splash[] = "May the best man win\0"; //Start screens splash message
    char *ptr1 = start_splash;
    tinygl_text(ptr1); //function that displays the message

    pacer_init (GAME_START_PACER_RATE); //Rate at which the message is shown

    while (!navswitch_push_event_p(4))
     /* Waits for the player to press the nav switch. 
     If the nav switch is pushed then the loop is broken and the game starts */
    {
        pacer_wait();
        tinygl_update();
        navswitch_update();

    }
    display_clear();
    navswitch_update();
}


static void IO_receive(void)
/*Function that handles the data received from the oppositions board; 
Receives the inversed column to where the opponents bullet was fired and checks for a special integer to signify a win; 
Checks that the bullet sent from the opponent is in the same column as the players' ship,
if so then the function removes one pixel away from the opponent;
If the players' pixel count is less than 0, then the players 'LOST' status is set to true and a special integer value ('WINNER_CHECK') is
sent to tell the opponent that they have won. The games 'end_function' is then called from the main function.
If the player receives the special value, then the players 'WON' status is set to true, and the game is over (end_display function is activated)*/
{

    uint8_t received_num = ir_uart_getc();
    damage_col = received_num;
    if (received_num == myShipPtr->head_column) {

        single_pixel_set(display,myShipPtr->head_column,myShipPtr->head_row,0);

        if (myShipPtr->head_row < LEDMAT_ROWS) {
            myShipPtr->head_row ++; //The ships head row is increased which decreases the height of the ship
        } else {
            LOST = true;
            for (uint8_t i =0; i<GAME_OVER_CHECK;i++) {
                ir_uart_putc(WINNER_CHECK); //The player loses. The special integer is sent 5 times to counter IR problems
            }
        }

    } else if (received_num == WINNER_CHECK) {
        WON = true; /*'WINNER_CHECK' is sent from the oppositions board if they have lost. When received, the players' 'WON' status is set to true signifying that they have won the game*/
    }


}


static void IO_send(uint8_t col)
/*This function sends the opponent the inverse of the column that the player had shot the
bullet in. This is becaused the game is designed to be played with both players' Funkits opposite 
each other*/
{

    uint8_t inversed_col = LEDMAT_COLS-col;
    ir_uart_putc(inversed_col);

}


static void IO_update(void)
/*Function checks to see if the player has shot a bullet that needs to be sent to the opponent
or if there is a bullet from the opponent thats ready to be received*/
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
/*Function that checks what direction the player has moved the navswitch*/
{
    navswitch_update();
    if (navswitch_push_event_p(NAVSWITCH_EAST)) {
        //if pushed right, the functions to move the ship to the right are called.
        column_shift_right(display,myShipPtr->head_column,myShipPtr->head_row);
        ship_to_right(myShipPtr);
    } else if (navswitch_push_event_p(NAVSWITCH_WEST)) {
        //if pushed left, the functions to move the ship to the left are called.
        column_shift_left(display,myShipPtr->head_column,myShipPtr->head_row);
        ship_to_left(myShipPtr);
    } else if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
        //if pushed in, the bullet is 'shot' by setting the bullet column to the column that the ship is in and makes the bullet column available to send 
        bullet_col = (myShipPtr->head_column);
        bullet_row = (myShipPtr->head_row)-2;
        SHOOTING_STATUS = true;
        SEND = true;
    }
}



static void bullet_shoot(void)
/*Function that handles the shooting of the ships bullet*/
{
    // Turning off the led from previous bullet row, turn on the next intended led;
    if (SHOOTING_STATUS && bullet_row != 0){
        single_pixel_set(display,bullet_col,bullet_row+1 ,0);
        single_pixel_set(display,bullet_col,bullet_row ,1);
        bullet_row--;
    }

    //Change the status back to not shooting when bullet reaches to row 0;
    else if (SHOOTING_STATUS && bullet_row == 0) {
        single_pixel_set(display,bullet_col,1,0);
        single_pixel_set(display,bullet_col,0,1);
        SHOOTING_STATUS = false;
    }

    // while not shooting, turn off row 0 led;
    else if (!SHOOTING_STATUS) {
        single_pixel_set(display,bullet_col,0,0);
    }

}

static void bullet_receive(void)
{
    uint8_t bullet_end_row = LEDMAT_ROWS;

    if (damage_col == myShipPtr->head_column) {
        bullet_end_row = myShipPtr->head_row;
    }

    if (RECEIVED && damage_row == 0) {
        single_pixel_set(display,damage_col,0,1);
        damage_row++;
    }

    else if (RECEIVED && damage_row < bullet_end_row ){
        single_pixel_set(display,damage_col,damage_row-1,0);
        single_pixel_set(display,damage_col,damage_row,1);
        damage_row++;
    }

    else if (RECEIVED && damage_row == bullet_end_row) {
        single_pixel_set(display,damage_col,damage_row-1,0);
        damage_row =0;
        RECEIVED = false;
    }


}

static void end_display(void)
/*The last function called by the program, which is the end of the game. It displays a message on 
a players' board based on whether they have won or lost*/
{


    tinygl_init (GAME_START_PACER_RATE);
    tinygl_font_set (&font5x7_1);
    tinygl_text_speed_set (START_MESSAGE_RATE);
    tinygl_text_mode_set (TINYGL_TEXT_MODE_SCROLL);

    char displayWon[] = "Winner\0"; //End splash message for the winner 
    char displayLose[] = "Loser\0"; //End splash message for the loser
    char *ptrWon =displayWon ;
    char *ptrLost =displayLose ;
    if (WON) {                      //Displays the corresponding messages depending on whether the player won or lost
        tinygl_text(ptrWon);
    } else if (LOST) {
        tinygl_text(ptrLost);
    }

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
    // Game/LED initialisation functions
    system_init ();
    display_init();
    ir_uart_init ();
    pacer_init(800);
    //Ship initialisation
    myShip = ship_init();
    myShipPtr = &myShip;
    //Calls to the start of the game
    game_start();
    //Sets the tickrate for the ledmat, navswitch, IO and bullet.
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

        //Updates the LED matrix to when its ticks are 
        if (update_ledmat_tick >=3) {
            update_ledmat_tick = 0;
            update_ledmat(&display);
        }

        if (switch_status_check_tick>=3) {
            switch_status_check_tick = 0;
            switch_status_check();
        }

        if (bullet_display_tick >=7) {
            bullet_display_tick = 0;
            bullet_shoot();
            bullet_receive();

        }

        if (IO_check_tick>=20) {
            IO_check_tick = 0;
            IO_update();
        }

        if (WON||LOST) {
            break;
        }



    }
    //Game is over as the players 'WON'/'LOST' values have been set to true, end game function is called that shows the winner/loser splashes
    end_display();
    return 0;
}
