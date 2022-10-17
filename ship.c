#include <stdlib.h>
#include "ship.h"

/* Number of initial values.  */
#define HEAD_ROW_INIT 4

#define HEAD_COLUMN_INIT 2

#define HEAD_SHOT_DOWN_INIT 0




ship_t ship_init(void)
{
    ship_t myShip = {HEAD_ROW_INIT, HEAD_COLUMN_INIT, HEAD_SHOT_DOWN_INIT};
    return myShip;
}


void ship_to_right(ship_t* aShip)
{
    if (aShip->head_column == 4) {
        aShip->head_column = 0;
    } else {
        aShip->head_column = (aShip->head_column) + 1;
    }
}

void ship_to_left(ship_t* aShip)
{
    if (aShip->head_column == 0) {
        aShip->head_column = 4;
    } else {
        aShip->head_column = (aShip->head_column - 1);
    }
}
