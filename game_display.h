/** @file   game_display.h
    @author Jiahe Bao, Callum Whitehead
    @date   01 October 2022
    @brief  The header file for game_display.c

    @defgroup
*/

#ifndef GAME_DISPLAY_H
#define GAME_DISPLAY_H

#include "system.h"


void column_shift_right(uint8_t* led_map, uint8_t ship_head_col,uint8_t ship_head_row);

void column_shift_left(uint8_t* led_map, uint8_t ship_head_col,uint8_t ship_head_row);

void update_ledmat(uint8_t* led_map);

void single_pixel_set (uint8_t* display,uint8_t col, uint8_t row, bool val);


#endif
