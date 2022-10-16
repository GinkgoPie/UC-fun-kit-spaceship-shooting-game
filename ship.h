/** @file   ship.h
    @author Jiahe Bao
    @date  15 october 2022
    @brief  Ship header file

    @defgroup
*/

#ifndef SHIP_H
#define SHIP_H

#include "system.h"

typedef struct
{
    uint8_t head_row;
    uint8_t head_column;
    bool head_shot_down;
} ship_t;

ship_t ship_init(void);

void ship_to_right(ship_t* aShip);

void ship_to_left(ship_t* aShip);

void ship_shoot(ship_t* aShip);


#endif
