/** @file   ship.c
    @author Jiahe Bao, Callum Whitehead
    @date   01 October 2022
    @brief  A file that initialises the players' ship, handles the position of the players' ship and shifts it left or right
            depending on what direction the player pushed the navswitch 

    @defgroup
*/

#include <stdlib.h>
#include "ship.h"

/* Number of initial values.  */
#define HEAD_ROW_INIT 4

#define HEAD_COLUMN_INIT 2




ship_t ship_init(void)
//Initialises the values for the players' ships' head row, column
{
    ship_t myShip = {HEAD_ROW_INIT, HEAD_COLUMN_INIT};
    return myShip;
}


void ship_to_right(ship_t* aShip)
/*Shifts the ship right by increasing the ships head columm. If the head column is already in 
the last column (col 4) when the funtion is called, then the head column becomes is shifted to the
first column*/
{
    if (aShip->head_column == 4) {
        aShip->head_column = 0;
    } else {
        aShip->head_column = (aShip->head_column) + 1;
    }
}

void ship_to_left(ship_t* aShip)
/*Shifts the ship left by increasing the ships head columm. If the head column is already in 
the first column (col 0) when the funtion is called, then the head column becomes is shifted to the
last column*/
{
    if (aShip->head_column == 0) {
        aShip->head_column = 4;
    } else {
        aShip->head_column = (aShip->head_column - 1);
    }
}
