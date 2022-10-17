
#include <stdlib.h>
#include "game_display.h"
#include "ledmat.h"
#include "display.h"
#include "led.h"
#include "pacer.h"


#define LEDMAT_ROWS 6
#define LEDMAT_COLS 4




void column_shift_right(uint8_t* led_map, uint8_t ship_head_col,uint8_t ship_head_row) {
    // uint8_t *copy = NULL;
    // *copy = led_map[LEDMAT_COLS];
    // led_map[4] = led_map[3] & 0xF0;
    // led_map[3] = led_map[2] & 0xF0 ;
    // led_map[2] = led_map[1] & 0xF0;
    // led_map[1] = led_map[0] & 0xF0;
    // led_map[0] = *copy & 0xF0;

    for (uint8_t i = ship_head_row; i<LEDMAT_ROWS+1;i++) {
        single_pixel_set(led_map,ship_head_col,i,0);
    }

    if (ship_head_col == 4) {
        ship_head_col = 0;
    } else {
        ship_head_col++;
    }

    for (uint8_t i = ship_head_row; i<LEDMAT_ROWS+1;i++) {
        single_pixel_set(led_map,ship_head_col,i,1);
    }


}


void column_shift_left(uint8_t* led_map, uint8_t ship_head_col,uint8_t ship_head_row) {
    // uint8_t *copy = NULL;
    // *copy = led_map[0];
    // led_map[0] = led_map[1]& 0xF0;
    // led_map[1] = led_map[2]& 0xF0;
    // led_map[2] = led_map[3]& 0xF0;
    // led_map[3] = led_map[4]& 0xF0;
    // led_map[4] = *copy& 0xF0;


    for (uint8_t i = ship_head_row; i<LEDMAT_ROWS+1;i++) {
        single_pixel_set(led_map,ship_head_col,i,0);
    }

    if (ship_head_col == 0) {
        ship_head_col = LEDMAT_COLS;
    } else {
        ship_head_col--;
    }

    for (uint8_t i = ship_head_row; i<LEDMAT_ROWS+1;i++) {
        single_pixel_set(led_map,ship_head_col,i,1);
    }
}

void update_ledmat (uint8_t* led_map)
{

    static uint8_t col = 0;

    ledmat_display_column (led_map[col], col);

    col++;
    if (col >= DISPLAY_WIDTH)
        col = 0;

}


void single_pixel_set (uint8_t* display,uint8_t col, uint8_t row, bool val)
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
