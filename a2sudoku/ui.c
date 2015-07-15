//
//  ui.c
//  a2sudoku
//
//  Created by Jeremy Rand on 2015-07-15.
//  Copyright (c) 2015 Jeremy Rand. All rights reserved.
//


#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <tgi.h>
#include <tgi/tgi-mode.h>

#include "game.h"
#include "ui.h"


// Extern symbols for graphics drivers
extern char a2e_hi;


// Macros and defines
#define SQUARE_WIDTH 29
#define SQUARE_HEIGHT 19

#define THICK_LINE_WIDTH 2
#define THIN_LINE_WIDTH 1
#define THICK_LINE_HEIGHT 2
#define THIN_LINE_HEIGHT 1

#define TEXT_OFFSET_X 12
#define TEXT_OFFSET_Y 6

#define TOTAL_WIDTH ((BOARD_SIZE * SQUARE_WIDTH) + \
                    ((BOARD_SIZE + 1) * THIN_LINE_WIDTH) + \
                    (((BOARD_SIZE / SUBSQUARE_SIZE) + 1) * (THICK_LINE_WIDTH - THIN_LINE_WIDTH)))

#define TOTAL_HEIGHT ((BOARD_SIZE * SQUARE_HEIGHT) + \
                     ((BOARD_SIZE + 1) * THIN_LINE_HEIGHT) + \
                     (((BOARD_SIZE / SUBSQUARE_SIZE) + 1) * (THICK_LINE_HEIGHT - THIN_LINE_HEIGHT)))


// Globals;

tPos cursorX, cursorY;
int screenSquaresX[BOARD_SIZE];
int screenSquaresY[BOARD_SIZE];


// Implementation


void initUI(void)
{
    static bool tgi_inited = false;
    tPos pos;
    int xPos = 0;
    int yPos = 0;
    
    if (tgi_inited)
        return;
    
    // Install drivers
    tgi_install(&a2e_hi);
    
    tgi_init();
    tgi_clear();
    
    tgi_setcolor(COLOR_WHITE);
    for (pos = 0; pos <= BOARD_SIZE; pos++) {
        if ((pos % SUBSQUARE_SIZE) == 0) {
            tgi_bar(xPos, 0, xPos + THICK_LINE_WIDTH - 1, TOTAL_HEIGHT - 1);
            xPos += THICK_LINE_WIDTH;
            
            tgi_bar(0, yPos, TOTAL_WIDTH - 1, yPos + THICK_LINE_HEIGHT - 1);
            yPos += THICK_LINE_HEIGHT;
        } else {
            tgi_bar(xPos, 0, xPos + THIN_LINE_WIDTH - 1, TOTAL_HEIGHT - 1);
            xPos += THIN_LINE_WIDTH;
            
            tgi_bar(0, yPos, TOTAL_WIDTH - 1, yPos + THIN_LINE_HEIGHT - 1);
            yPos += THIN_LINE_HEIGHT;
        }
        
        if (pos < BOARD_SIZE) {
            screenSquaresX[pos] = xPos;
            screenSquaresY[pos] = yPos;
        }
        
        xPos += SQUARE_WIDTH;
        yPos += SQUARE_HEIGHT;
    }
    
    tgi_inited = true;
}


void shutdownUI(void)
{
    // Uninstall drivers
    tgi_uninstall();
}


void displayInstructions(void)
{
    int seed = 0;
    
    clrscr();
    
    //               1111111111222222222233333333334
    //      1234567890123456789012345678901234567890
    printf(
           "           Apple ][ Sudoku\n"
           "            By Jeremy Rand\n"
           "\n"
           "The goal is to get the numbers from 1 to"
           "9 uniquely in each column, row and 3x3\n"
           "subsquare.  Move the cursor with I-J-K-L"
           "or the arrow keys.  Press a number key\n"
           "to enter a value.  Press a number key\n"
           "while holding shift or closed apple to\n"
           "toggle a scratch value.  Press 0 to\n"
           "clear a square.\n"
           "\n"
           "Play ends when the puzzle is solved.\n"
           "\n"
           "Press escape or Q to quit at any time.\n"
           "Press R to start a new game.\n"
           "Press H to see this info again.\n"
           "\n"
           "\n"
           "\n"
           "       Press any key to start");
    
    // The amount of time the user waits to read the in
    while (!kbhit())
        seed++;
    
    cgetc();
    srand(seed);
    
    clrscr();
}


void updatePos(tPos x, tPos y, tSquareVal val, tScratchValues scratch, bool correct, bool invalid)
{
    int screenX = screenSquaresX[x];
    int screenY = screenSquaresY[y];
    int edgeX = screenX + SQUARE_WIDTH - 1;
    int edgeY = screenY + SQUARE_HEIGHT - 1;
    char buffer[2];
    
    tgi_setcolor(COLOR_BLACK);
    tgi_bar(screenX, screenY, edgeX, edgeY);
    
    tgi_setcolor(COLOR_WHITE);
    if (val != EMPTY_SQUARE) {
        buffer[0] = '0' + val;
        buffer[1] = '\0';
        
        tgi_outtextxy(screenX + TEXT_OFFSET_X, screenY + TEXT_OFFSET_Y, buffer);
        
        if (!correct)
            tgi_line(screenX, edgeY, edgeX, screenY);
        
        if (invalid)
            tgi_line(screenX, edgeY, edgeX, screenY);
    }
    
    if ((cursorX == x) &&
        (cursorY == y)) {
        tgi_line(screenX, screenY, edgeX, screenY);
        tgi_lineto(edgeX, edgeY);
        tgi_lineto(screenX, edgeY);
        tgi_lineto(screenX, screenY);
    }
}


bool playGame(void)
{
    initUI();
    
    cursorX = 0;
    cursorY = 0;
    
    startGame(updatePos);
    
    while (true) {
        char ch = cgetc();
        
        switch (ch) {
            case 'h':
            case 'H':
                displayInstructions();
                clrscr();
                refreshAllPos();
                break;
                
            case 'r':
            case 'R':
                return true;
                
            case 'q':
            case 'Q':
            case CH_ESC:
                shutdownUI();
                return false;
                
            case 'i':
            case 'I':
            case CH_CURS_UP:
                if (cursorY != 0) {
                    cursorY--;
                    refreshPos(cursorX, cursorY);
                    refreshPos(cursorX, cursorY + 1);
                }
                break;
                
            case 'j':
            case 'J':
            case CH_CURS_LEFT:
                if (cursorX != 0) {
                    cursorX--;
                    refreshPos(cursorX, cursorY);
                    refreshPos(cursorX + 1, cursorY);
                }
                break;
                
            case 'k':
            case 'K':
            case CH_CURS_RIGHT:
                if (cursorX < BOARD_SIZE - 1) {
                    cursorX++;
                    refreshPos(cursorX, cursorY);
                    refreshPos(cursorX - 1, cursorY);
                }
                break;
                
            case 'm':
            case 'M':
            case CH_CURS_DOWN:
                if (cursorY < BOARD_SIZE - 1) {
                    cursorY++;
                    refreshPos(cursorX, cursorY);
                    refreshPos(cursorX, cursorY - 1);
                }
                break;
                
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                setValueAtPos(cursorX, cursorY, ch - '0');
                break;
        }
    }
    
    return false;
}