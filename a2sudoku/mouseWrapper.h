//
//  mouseWrapper.h
//  a2sudoku
//
//  Created by Jeremy Rand on 2018-06-24.
//  Copyright © 2018 Jeremy Rand. All rights reserved.
//

#ifndef _GUARD_PROJECTa2sudoku_FILEmouseWrapper_
#define _GUARD_PROJECTa2sudoku_FILEmouseWrapper_

#include <stdbool.h>

#include "puzzles.h"


// Typedefs

typedef void (*tMouseCallback)(tPos newX, tPos newY);


// API

extern bool initMouse(tMouseCallback callback);
extern void shutdownMouse(void);
extern void pollMouse(void);
extern void moveMouseToPos(tPos newX, tPos newY);


#endif /* define _GUARD_PROJECTa2sudoku_FILEmouseWrapper_ */
