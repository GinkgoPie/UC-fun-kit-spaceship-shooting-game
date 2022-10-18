/** @file   ship.h
    @author Jiahe Bao, Callum Whitehead
    @date  15 October 2022
    @brief  Header file for ship.c

    @defgroup
*/

#ifndef SHIP_H
#define SHIP_H

#include "system.h"

// Creates the ship, a struct called ship_t.
typedef struct
{
    uint8_t head_row;
    uint8_t head_column;

} ship_t;

ship_t ship_init(void);

void ship_to_right(ship_t* aShip);

void ship_to_left(ship_t* aShip);



#endif
