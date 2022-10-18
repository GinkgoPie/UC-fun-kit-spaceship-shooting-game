/** @file   game_display.c
    @author Jiahe Bao, Callum Whitehead
    @date   01 October 2022
    @brief  A file that is tasked with updating the display LED Matrix with the position
            of the players' ships location and also the displaying of the bullet that the player
            is shooting and being shot by  

    @defgroup
*/


#include <stdlib.h>
#include "game_display.h"
#include "ledmat.h"
#include "display.h"
#include "led.h"
#include "pacer.h"


#define LEDMAT_ROWS 6
#define LEDMAT_COLS 4


void column_shift_right(uint8_t* led_map, uint8_t ship_head_col,uint8_t ship_head_row) 
//Function that shifts the 'ship' right on the LED Matrix.
{
    for (uint8_t i = ship_head_row; i<LEDMAT_ROWS+1;i++) {  // Sets the previous 'ship' pixels to be off.
        single_pixel_set(led_map,ship_head_col,i,0);
    }

    if (ship_head_col == 4) { 
        ship_head_col = 0;      //Sets the ships column to be shown in the first column if the ships head is in a column off the LED Matrix 
    } else {
        ship_head_col++;  //Else: increase the value of the ships column in order to shift the ship right
    }

    for (uint8_t i = ship_head_row; i<LEDMAT_ROWS+1;i++) { // Moves the ships column display to the right by setting the pixels to the right of the ships previous column on.  
        single_pixel_set(led_map,ship_head_col,i,1);
    }
}


void column_shift_left(uint8_t* led_map, uint8_t ship_head_col,uint8_t ship_head_row) {
//Function the same as the column_shift_right function but shifts left by decreasing the value of the ships column
    for (uint8_t i = ship_head_row; i<LEDMAT_ROWS+1;i++) {
        single_pixel_set(led_map,ship_head_col,i,0);
    }

    if (ship_head_col == 0) {
        ship_head_col = LEDMAT_COLS;
    } else {
        ship_head_col--;    //Decreases the value of the ships column in order to shift the ship right
    }

    for (uint8_t i = ship_head_row; i<LEDMAT_ROWS+1;i++) {
        single_pixel_set(led_map,ship_head_col,i,1);
    }
}

void update_ledmat (uint8_t* led_map)
//Constantly updates the LED Matrix with the players' ships current position
{

    static uint8_t col = 0;

    ledmat_display_column (led_map[col], col);

    col++;
    if (col >= DISPLAY_WIDTH)
        col = 0;    //if the ships column is more than the amount of columns in the matrix, then the ship column becomes 0 (the firt column in the Matrix)

}


void single_pixel_set (uint8_t* display,uint8_t col, uint8_t row, bool val)
//Sets a singular pixel to be either on or off depending on the row and column of the pixel in the LED Matrix
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
