//
//  game.h
//  a2sudoku
//
//  Created by Jeremy Rand on 2015-07-15.
//  Copyright (c) 2015 Jeremy Rand. All rights reserved.
//


#include "puzzles.h"

#ifndef __a2sudoku__game__
#define __a2sudoku__game__


// Macros

#define SCRATCH_TEST(scratch, val) ((scratch) & (0x1 << val))


// Typedefs

typedef uint16_t tScratchValues;
typedef void (*tUpdatePosCallback)(tPos x, tPos y, tSquareVal val, tScratchValues scratch, bool correct, bool invalid, bool knownAtStart);


// API

extern void startGame(tDifficulty difficulty, tUpdatePosCallback callback);

extern void refreshAllPos(void);

extern void refreshPos(tPos x, tPos y);

extern bool isPuzzleSolved(void);

// Returns false if unable to set the value for some reason
extern bool setValueAtPos(tPos x, tPos y, tSquareVal val);

// Returns false if unable to toggle the scratch value for some reason
extern bool toggleScratchValueAtPos(tPos x, tPos y, tSquareVal val);


#endif /* defined(__a2sudoku__game__) */
