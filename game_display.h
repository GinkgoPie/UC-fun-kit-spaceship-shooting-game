/** @file   ship.h
    @author Jiahe Bao
    @date  15 october 2022
    @brief  Updating and displaying led matrix during the game

    @defgroup
*/

#ifndef GAME_DISPLAY_H
#define GAME_DISPLAY_H

#include "system.h"




void column_shift_right(uint8_t* led_map);


void column_shift_left(uint8_t* led_map);


void update_ledmat(uint8_t* led_map);

void single_pixel_set (uint8_t* display,uint8_t col, uint8_t row, bool val);





#endif
