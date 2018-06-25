//
//  mouseWrapper.c
//  a2sudoku
//
//  Created by Jeremy Rand on 2018-06-24.
//  Copyright © 2018 Jeremy Rand. All rights reserved.
//

#include "mouseWrapper.h"
#include "drivers/a2_mouse_drv.h"

// Defines and macros

#define MOUSE_POS_SHIFT 4
#define MOUSE_GRID_SIZE (BOARD_SIZE << MOUSE_POS_SHIFT)


// Globals

static tMouseCallback gCursorPosCallback = NULL;
static bool gMouseInstalled = false;
static bool gMouseInPoll = false;
static struct mouse_box gMouseBox = { 0, 0, MOUSE_GRID_SIZE - 1, MOUSE_GRID_SIZE - 1 };


// Implementation

bool initMouse(tMouseCallback callback)
{
    if (!gMouseInstalled) {
        if (mouse_install(&mouse_def_callbacks, &a2_mouse_drv) == 0) {
            gMouseInstalled = true;
            
            mouse_setbox(&gMouseBox);
            moveMouseToPos(0, 0);
        }
    }
    
    gCursorPosCallback = callback;
    
    return gMouseInstalled;
}


void shutdownMouse(void)
{
    if (gMouseInstalled) {
        mouse_uninstall();
        gMouseInstalled = false;
    }
}


void pollMouse(void)
{
    struct mouse_info mouseInfo;
    
    if (!gMouseInstalled)
        return;
    
    mouse_info(&mouseInfo);
    
    gMouseInPoll = true;
    gCursorPosCallback(mouseInfo.pos.x >> MOUSE_POS_SHIFT,
                       mouseInfo.pos.y >> MOUSE_POS_SHIFT);
    gMouseInPoll = false;
}


void moveMouseToPos(tPos newX, tPos newY)
{
    if (!gMouseInstalled)
        return;
    
    if (gMouseInPoll)
        return;
    
    mouse_move(newX << MOUSE_POS_SHIFT, newY << MOUSE_POS_SHIFT);
}
