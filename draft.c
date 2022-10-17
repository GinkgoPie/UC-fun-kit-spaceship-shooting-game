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
#define ledmat_rows 6
#define ledmat_cols 4
#define END_MAGIC 127

uint8_t display[DISPLAY_WIDTH] =
{
    0x00, 0x00, 0x70, 0x00, 0x00
};

static uint8_t cockpit[2] = {4,2}; // Initially, The head location of our ship is at row 5 column 3;
static bool SHOOTING_STATUS = false;
static bool SEND = false;
static bool WIN_STATUS = false;
static uint8_t HP_NUMBER = 3;
static bool GAME_OVER = false;


static void game_start (void) {
    //This is the games' start splash screen that shows a scrolling message and waits for the player to press the nav switch
    tinygl_init (GAME_START_PACER_RATE);
    tinygl_font_set (&font5x7_1);
    tinygl_text_speed_set (START_MESSAGE_RATE);
    tinygl_text_mode_set (TINYGL_TEXT_MODE_SCROLL);

    char display[] = "MAY THE BEST SHIP WIN\0"; //The message that is shown
    char *ptr1 = display;
    tinygl_text(ptr1);

    pacer_init (GAME_START_PACER_RATE);


    while (!navswitch_push_event_p(4))
    //Waits for the player to press the nav switch. If the nav switch is pushed then the loop is broken and the game starts
    {
        pacer_wait();
        tinygl_update();
        navswitch_update();

    }
    display_clear();
    navswitch_update();
}

static void update_ledmat (void)
{
    //Constantly update the LED Matrix with the players' ships current position 

    static uint8_t col = 0;

    ledmat_display_column (display[col], col);

    col++;
    if (col >= DISPLAY_WIDTH)
        col = 0;

}


static void column_shift_right(void) {
    //Function that shifts the 'ship' right on the LED Matrix
    uint8_t *copy = NULL;
    *copy = display[LEDMAT_COLS_NUM-1];
    display[4] = display[3];
    display[3] = display[2];
    display[2] = display[1];
    display[1] = display[0];
    display[0] = *copy;
    if (cockpit[1] == 4) {
        cockpit[1] = 0; //if the ships column position is a value is in a column not on the LED Matrix, then the ship is shown in the opposite column
    } else {
        cockpit[1] = (cockpit[1] + 1); //else the ship is displayed in the column to the right
    }
}

static void column_shift_left(void) {
    //Function that shifts the 'ship' left on the LED Matrix
    uint8_t *copy = NULL;
    *copy = display[0];
    display[0] = display[1];
    display[1] = display[2];
    display[2] = display[3];
    display[3] = display[4];
    display[4] = *copy;
    if (cockpit[1] == 0) {
        cockpit[1] = 4; //if the ships column position is a value is in a column not on the LED Matrix, then the ship is shown in the opposite column
    } else {
        cockpit[1] = (cockpit[1] - 1); //else the ship is displayed in the column to the left
    }

}

static void game_end(void)
//Function that the loser goes to which sends the 'magic number' to the opponents board that tells them that they've won
{
    HP_NUMBER = 0;
    ir_uart_putc(END_MAGIC);

}

void single_pixel_set (uint8_t col, uint8_t row, bool val)
//Function the deals with taking one pixel away from the ship
{
    uint8_t bitmask;
    uint8_t pattern;

    if (col >= DISPLAY_WIDTH || row >= DISPLAY_HEIGHT)
        return;

    bitmask = BIT (row);
    pattern = display[col] & ~bitmask;

    if (val)
        pattern |= bitmask;

    display[col] = pattern;
}


static void IO_receive(void)
{
    //Function that receives the transmitted data (bullet) from the opponents board.
    uint8_t received_num = ir_uart_getc();

    //if the received bullet is in the same column as the players' ship, one 'hp' is taken away from the ship, meaning one pixel of the ship gets taken away.
    if (received_num == cockpit[1]) {
        single_pixel_set(cockpit[1],cockpit[0],0);
        HP_NUMBER--;

        if (cockpit[0] < 6) {
            cockpit[0] ++;
        } else  {
            /*
            If the players' ship 'hp' is zero, then the game is over and is sent to the 'game_end' function 
            which sends a special number to the opponents board that tells them that they've won, and also sends the player to the losers splash screen.
            */
            WIN_STATUS = true;
            game_end(); //I lose
        }
    } else if (received_num == END_MAGIC) {
        //If the player receives the special number from the opponent, then the player has won and sets the game to be over, which activates the winners splash screen
        WIN_STATUS = true;
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
    //Checks if the players' navswitch was pushed (bullet has been fired). If true then the bullet is sent to the opponent
      IO_send(cockpit[1]);
      SEND = false;
   }
}


static void switch_status_check (void)
{
    //Function that checks if the navswitch has been activated, and then what 'direction' it was pressed
    navswitch_update();
    if (navswitch_push_event_p(NAVSWITCH_EAST)) {
        //if pressed right, the program goes to the function that shifts the 'ship' right
        column_shift_right();
    } else if (navswitch_push_event_p(NAVSWITCH_WEST)) {
        //if pressed right, the program goes to the function that shifts the 'ship' left
        column_shift_left();
    } else if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
        //if pressed in, the program shoots the 'ship's' bullet and makes the bullet available to be sent to the opponent
        SHOOTING_STATUS = true;
        SEND = true;
    }

}



static void ship_shoot(void)
/* 
    Function that makes the ship shoot by checking if the nav button has pressed by the 'SHOOTING_STATUS'
    variable which made true when the button is pressed.
*/
{
  uint8_t column = cockpit[1];
  uint8_t pattern1 = display[column]|(1<<(7-cockpit[0]-1));
  uint8_t pattern2 = display[column]|(1);
  if (SHOOTING_STATUS){
      for (uint8_t i =cockpit[0]; i>0; i--) {
          ledmat_display_column (pattern1, column);
          pacer_wait();
          pattern1 |= (1<<i);
      }
      ledmat_display_column (pattern2, column);
      SHOOTING_STATUS = false;
    }   
}

//Functions that set the LED
void led_on(void) 
{
    PORTC |= (1 << 2);
}

void led_off(void) 
{
    PORTC &= ~(1 << 2);
}


int main (void)
{

    system_init ();
    display_init();
    ir_uart_init ();
    pacer_init(400);

    game_start();

    uint8_t update_ledmat_tick = 0;
    uint8_t switch_status_check_tick = 0;
    uint8_t IO_check_tick = 0;
    uint8_t shooting_check_tick = 0;
    while(1)
    {
        /*
        This loop starts off in the state being that the game is 
        playing (!GAME_OVER), which loops through the basic game functions. 
        Then as the condition of GAME_OVER becomes true, the program activates the
        game over splash screens which show two different messages for the winner and the loser.
        The game can then be restarted by pressing the nav switch.
        */
        if (!GAME_OVER) {
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

            if (WIN_STATUS) {
                GAME_OVER = true;
            }

        } else if (GAME_OVER) {
            //The loser's splash screen
            if (HP_NUMBER == 0) {
                tinygl_clear();
                tinygl_text_speed_set(1);
                tinygl_text("LOSER\0");
                while (GAME_OVER == true) {
                    tinygl_update();
                    navswitch_update();
                    //if the navswitch is pressed, then the game restarts by resetting the values for the ship
                    if(navswitch_push_event_p(NAVSWITCH_PUSH)) {
                        //Resets the players' game to its game start values when the player presses the navswitch
                        tinygl_clear();
                        WIN_STATUS = false;
                        GAME_OVER = false;
                        display[2] = 0x70;
                        cockpit[0] = 4;
                        cockpit[1] = 2;                        
                        HP_NUMBER = 3;
                    }
                }
            } else if (HP_NUMBER > 0){
                //The winner's splash screen
                led_on ();
                tinygl_clear();
                tinygl_text_speed_set(1);
                tinygl_text("WINNER\0");
                while (GAME_OVER == true) {
                    tinygl_update();
                    navswitch_update();
                    if(navswitch_push_event_p(NAVSWITCH_PUSH)) {
                        //Resets the players' game to its game start values when the player presses the navswitch
                        led_off ();
                        tinygl_clear();
                        WIN_STATUS = false;
                        GAME_OVER = false;
                        for (uint8_t i = 0; i < 5; i++) {
                            display[i] = 0x00;
                        }
                        display[2] = 0x70;
                        cockpit[0] = 4;
                        cockpit[1] = 2;  
                        HP_NUMBER = 3;
                    }
                }
            }
        }

    }
    return 0;
}
