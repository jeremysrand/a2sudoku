//
//  puzzles.h
//  a2sudoku
//
//  Created by Jeremy Rand on 2015-07-15.
//  Copyright (c) 2015 Jeremy Rand. All rights reserved.
//


#include <stdbool.h>
#include <stdint.h>


#ifndef __a2sudoku__puzzles__
#define __a2sudoku__puzzles__


// Defines

// A subsquare is the 3x3 square inside which each number appears once
#define SUBSQUARE_SIZE 3

// The board has dimensions of the square of the subsquare size
#define BOARD_SIZE (SUBSQUARE_SIZE * SUBSQUARE_SIZE)

#define EMPTY_SQUARE 0


// Forward declarations

struct tPuzzle;


// Typedefs

typedef uint8_t tSquareVal;
typedef uint8_t tPos;


// API

extern struct tPuzzle *getRandomPuzzle(void);

extern tSquareVal getStartValueAtPos(struct tPuzzle *puzzle, tPos x, tPos y);

extern bool checkValueAtPos(struct tPuzzle *puzzle, tSquareVal val, tPos x, tPos y);

extern void printPuzzle(struct tPuzzle *puzzle, bool solution);


#endif /* defined(__a2sudoku__puzzles__) */
