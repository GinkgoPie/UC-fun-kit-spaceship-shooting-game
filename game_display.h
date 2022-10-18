/** @file   game_display.h
    @author Jiahe Bao, Callum Whitehead
    @date   01 October 2022
    @brief  The header file for game_display.c

    @defgroup
*/

#ifndef GAME_DISPLAY_H
#define GAME_DISPLAY_H

#include "system.h"

//Function that shifts the 'ship' right on the LED Matrix.
void column_shift_right(uint8_t* led_map, uint8_t ship_head_col,uint8_t ship_head_row);

//Works the same as the column_shift_right function but shifts left by decreasing the value of the ships column
void column_shift_left(uint8_t* led_map, uint8_t ship_head_col,uint8_t ship_head_row);

//Constantly updates the LED Matrix with the players' ships current position
void update_ledmat(uint8_t* led_map);

//Sets a singular pixel to be either on or off depending on the row and column of the pixel in the LED Matrix
void single_pixel_set (uint8_t* display,uint8_t col, uint8_t row, bool val);


#endif
