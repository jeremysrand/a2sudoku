//
//  puzzles.c
//  a2sudoku
//
//  Created by Jeremy Rand on 2015-07-15.
//  Copyright (c) 2015 Jeremy Rand. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

#include "puzzles.h"


// Macros

#define PUZZLE_SOLVED_VAL(square) ((square) & 0xf)
#define PUZZLE_START_VAL(square) (((square) & 0x10) ? PUZZLE_SOLVED_VAL(square) : EMPTY_SQUARE)

#define PUZZLE_SQUARE(puzzle, x, y) ((puzzle)->squares[((y) * BOARD_SIZE) + (x)])

// PVAL is used to record the value of a square in a puzzle which is unknown at the start of the game.
// PVAL is short for "puzzle value"
#define PVAL(value) ((value) & 0xf)

// SVAL is used to record the value of a square in a puzzle which is known at the start of the game.
// SVAL is short for "starting value"
#define SVAL(value) (((value) & 0xf) | 0x10)


// Typedefs

typedef uint8_t tPuzzleNum;

typedef struct tPuzzle {
    tSquareVal squares[BOARD_SIZE * BOARD_SIZE];
} tPuzzle;


// Forward declarations

tPuzzle puzzles[];

tPuzzleNum numPuzzles(void);


// Implementation

tPuzzle *getRandomPuzzle(void)
{
    tPuzzleNum randomPuzzleNum = (rand() % numPuzzles());
    return &(puzzles[randomPuzzleNum]);
}



tSquareVal getStartValueAtPos(tPuzzle *puzzle, tPos x, tPos y)
{
    return PUZZLE_START_VAL(PUZZLE_SQUARE(puzzle, x, y));
}


bool checkValueAtPos(tPuzzle *puzzle, tSquareVal val, tPos x, tPos y)
{
    return (PUZZLE_SOLVED_VAL(PUZZLE_SQUARE(puzzle, x, y)) == val);
}


void printPuzzle(tPuzzle *puzzle, bool solution)
{
    tPos x, y;
    
    for (y = 0; y < BOARD_SIZE; y++) {
        if ((y % SUBSQUARE_SIZE) == 0) {
            printf("\n");
        }
        
        for (x = 0; x < BOARD_SIZE; x++) {
            char displayChar = ' ';
            tSquareVal squareVal;
            
            if ((x % SUBSQUARE_SIZE) == 0) {
                printf("  ");
            }
            if (solution) {
                squareVal = PUZZLE_SOLVED_VAL(PUZZLE_SQUARE(puzzle, x, y));
            } else {
                squareVal = PUZZLE_START_VAL(PUZZLE_SQUARE(puzzle, x, y));
            }
            
            if (squareVal != EMPTY_SQUARE)
                displayChar = '0' + squareVal;
            
            printf(" %c ", displayChar);
        }
        printf("\n");
    }
}


// Puzzle definitions

tPuzzle puzzles[] = {
    {
        {
            PVAL(4),PVAL(3),PVAL(5),  SVAL(2),SVAL(6),PVAL(9),  SVAL(7),PVAL(8),SVAL(1),
            SVAL(6),SVAL(8),PVAL(2),  PVAL(5),SVAL(7),PVAL(1),  PVAL(4),SVAL(9),PVAL(3),
            SVAL(1),SVAL(9),PVAL(7),  PVAL(8),PVAL(3),SVAL(4),  SVAL(5),PVAL(6),PVAL(2),
            
            SVAL(8),SVAL(2),PVAL(6),  SVAL(1),PVAL(9),PVAL(5),  PVAL(3),SVAL(4),PVAL(7),
            PVAL(3),PVAL(7),SVAL(4),  SVAL(6),PVAL(8),SVAL(2),  SVAL(9),PVAL(1),PVAL(5),
            PVAL(9),SVAL(5),PVAL(1),  PVAL(7),PVAL(4),SVAL(3),  PVAL(6),SVAL(2),SVAL(8),
            
            PVAL(5),PVAL(1),SVAL(9),  SVAL(3),PVAL(2),PVAL(6),  PVAL(8),SVAL(7),SVAL(4),
            PVAL(2),SVAL(4),PVAL(8),  PVAL(9),SVAL(5),PVAL(7),  PVAL(1),SVAL(3),SVAL(6),
            SVAL(7),PVAL(6),SVAL(3),  PVAL(4),SVAL(1),SVAL(8),  PVAL(2),PVAL(5),PVAL(9)
        }
    },
};


tPuzzleNum numPuzzles(void)
{
    return sizeof(puzzles) / sizeof(puzzles[0]);
}