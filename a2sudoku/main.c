/*
 * main.c
 * a2sudoku
 *
 * Created by Jeremy Rand on 2015-07-15.
 * Copyright (c) 2015 Jeremy Rand. All rights reserved.
 *
 */


#include <stdio.h>
#include <conio.h>

#include "ui.h"


int main(void)
{
    displayInstructions();
    
    while (playGame()) {
        // Loop back and play another game until playGame() returns false.
    }
    
    return 0;
}
