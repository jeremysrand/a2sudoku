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

#define TEXT_UNDERLINE_OFFSET_X -2
#define TEXT_UNDERLINE_OFFSET_Y 9
#define TEXT_UNDERLINE_WIDTH 8

#define TOTAL_WIDTH ((BOARD_SIZE * SQUARE_WIDTH) + \
                    ((BOARD_SIZE + 1) * THIN_LINE_WIDTH) + \
                    (((BOARD_SIZE / SUBSQUARE_SIZE) + 1) * (THICK_LINE_WIDTH - THIN_LINE_WIDTH)))

#define TOTAL_HEIGHT ((BOARD_SIZE * SQUARE_HEIGHT) + \
                     ((BOARD_SIZE + 1) * THIN_LINE_HEIGHT) + \
                     (((BOARD_SIZE / SUBSQUARE_SIZE) + 1) * (THICK_LINE_HEIGHT - THIN_LINE_HEIGHT)))

#define WON_WIDTH 100
#define WON_HEIGHT 55

#define SCRATCH_WIDTH 9
#define SCRATCH_HEIGHT 6


// Typedefs

typedef struct tOptions {
    tDifficulty difficulty;
    bool showInvalid;
    bool showWrong;
} tOptions;


// Globals;

tPos cursorX, cursorY;
int screenSquaresX[BOARD_SIZE];
int screenSquaresY[BOARD_SIZE];

tOptions gameOptions = {
    DIFFICULTY_EASY,
    true,
    true
};


// Implementation


char *difficultyString(tDifficulty difficulty)
{
    if (difficulty == DIFFICULTY_EASY)
        return "Easy";
    else if (difficulty == DIFFICULTY_MEDIUM)
        return "Medium";
    
    return "Hard";
}


void drawGrid(void)
{
    tPos pos;
    int xPos = 0;
    int yPos = 0;

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
}


void initUI(void)
{
    static bool tgi_inited = false;
    
    if (tgi_inited)
        return;
    
    // Install drivers
    tgi_install(&a2e_hi);
    
    tgi_init();
    
    tgi_inited = true;
}


void loadOptions(void)
{
    static bool optionsLoaded = false;
    FILE *optionsFile;
    
    if (optionsLoaded)
        return;

    optionsFile = fopen("a2sudokuopts", "rb");
    if (optionsFile != NULL) {
        fread(&gameOptions, sizeof(gameOptions), 1, optionsFile);
        fclose(optionsFile);
    }
    optionsLoaded = true;
}


void shutdownUI(void)
{
    FILE *optionsFile;
    optionsFile = fopen("a2sudokuopts", "wb");
    if (optionsFile != NULL) {
        fwrite(&gameOptions, sizeof(gameOptions), 1, optionsFile);
        fclose(optionsFile);
    }
    
    // Uninstall drivers
    tgi_uninstall();
}


void textMode(void)
{
    asm ("STA %w", 0xc051);
}


void graphicsMode(void)
{
    asm ("STA %w", 0xc050);
}


bool setOptions(void)
{
    bool shouldUpdate = false;
    bool keepLooping = true;
    
    while (keepLooping) {
        clrscr();
        
        //               1111111111222222222233333333334
        //      1234567890123456789012345678901234567890
        printf(
               "           Apple ][ Sudoku\n"
               "            By Jeremy Rand\n"
               "\n"
               "Options:\n"
               "    Difficulty          : %s\n"
               "    Show invalid values : %s\n"
               "    Show wrong values   : %s\n"
               "\n"
               "Press D to change difficulty.\n"
               "Press I to %sshow invalid values.\n"
               "Press W to %sshow wrong values.\n"
               "\n"
               "Press any other key to return.\n",
               difficultyString(gameOptions.difficulty),
               (gameOptions.showInvalid ? "On" : "Off"),
               (gameOptions.showWrong ? "On" : "Off"),
               (gameOptions.showInvalid ? "not " : ""),
               (gameOptions.showWrong ? "not " : ""));
        
        switch (cgetc()) {
            case 'd':
            case 'D':
                if (gameOptions.difficulty == DIFFICULTY_HARD)
                    gameOptions.difficulty = DIFFICULTY_EASY;
                else
                    gameOptions.difficulty++;
                break;
                
            case 'i':
            case 'I':
                gameOptions.showInvalid = !gameOptions.showInvalid;
                shouldUpdate = true;
                break;
                
            case 'w':
            case 'W':
                gameOptions.showWrong = !gameOptions.showWrong;
                shouldUpdate = true;
                break;
                
            default:
                keepLooping = false;
                break;
        }
    }
    
    clrscr();
    
    return shouldUpdate;
}


void displayInstructions(void)
{
    int seed = 0;
    char ch;
    
    loadOptions();
    
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
           "while holding shift or open apple to\n"
           "toggle a scratch value.  Press 0 to\n"
           "clear a square.  Play ends when the\n"
           "puzzle is solved.\n"
           "\n"
           "    Difficulty          : %s\n"
           "    Show invalid values : %s\n"
           "    Show wrong values   : %s\n"
           "\n"
           "Press escape or Q to quit at any time.\n"
           "Press O to change options.\n"
           "Press R to start a new game.\n"
           "Press H to see this info again.\n"
           "\n"
           " Press O to change options or any other\n"
           "              key to start",
           difficultyString(gameOptions.difficulty),
           (gameOptions.showInvalid ? "On" : "Off"),
           (gameOptions.showWrong ? "On" : "Off"));
    
    // The amount of time the user waits to read the in
    while (!kbhit())
        seed++;
    
    ch = cgetc();
    srand(seed);
    
    if ((ch == 'o') ||
        (ch == 'O'))
        setOptions();
    
    clrscr();
}


void drawOne(int screenX, int screenY)
{
    tgi_line(screenX + 2, screenY, screenX + 2, screenY + 3);
    tgi_line(screenX + 1, screenY + 4, screenX + 3, screenY + 4);
    tgi_setpixel(screenX + 1, screenY + 1);
}


void drawTwo(int screenX, int screenY)
{
    tgi_setpixel(screenX, screenY + 1);
    tgi_line(screenX + 1, screenY, screenX + 3, screenY);
    tgi_line(screenX + 4, screenY + 1, screenX, screenY + 4);
    tgi_lineto(screenX + 4, screenY + 4);
}


void drawThree(int screenX, int screenY)
{
    tgi_line(screenX + 1, screenY, screenX + 3, screenY);
    tgi_line(screenX + 1, screenY + 2, screenX + 3, screenY + 2);
    tgi_line(screenX + 1, screenY + 4, screenX + 3, screenY + 4);
    tgi_setpixel(screenX + 4, screenY + 1);
    tgi_setpixel(screenX + 4, screenY + 3);
}


void drawFour(int screenX, int screenY)
{
    tgi_line(screenX, screenY + 1, screenX, screenY + 2);
    tgi_lineto(screenX + 4, screenY + 2);
    tgi_line(screenX + 3, screenY, screenX + 3, screenY + 4);
}


void drawFive(int screenX, int screenY)
{
    tgi_line(screenX + 4, screenY, screenX, screenY);
    tgi_lineto(screenX, screenY + 2);
    tgi_lineto(screenX + 3, screenY + 2);
    tgi_line(screenX, screenY + 4, screenX + 3, screenY + 4);
    tgi_setpixel(screenX + 4, screenY + 3);
}


void drawSix(int screenX, int screenY)
{
    tgi_line(screenX + 3, screenY, screenX, screenY + 3);
    tgi_line(screenX + 1, screenY + 4, screenX + 3, screenY + 4);
    tgi_line(screenX + 1, screenY + 2, screenX + 3, screenY + 2);
    tgi_setpixel(screenX + 4, screenY + 3);
}


void drawSeven(int screenX, int screenY)
{
    tgi_line(screenX, screenY, screenX + 4, screenY);
    tgi_line(screenX + 4, screenY + 1, screenX + 1, screenY + 4);
}


void drawEight(int screenX, int screenY)
{
    tgi_line(screenX + 1, screenY, screenX + 3, screenY);
    tgi_line(screenX + 1, screenY + 2, screenX + 3, screenY + 2);
    tgi_line(screenX + 1, screenY + 4, screenX + 3, screenY + 4);
    tgi_setpixel(screenX, screenY + 1);
    tgi_setpixel(screenX, screenY + 3);
    tgi_setpixel(screenX + 4, screenY + 1);
    tgi_setpixel(screenX + 4, screenY + 3);
}


void drawNine(int screenX, int screenY)
{
    tgi_line(screenX + 1, screenY, screenX + 3, screenY);
    tgi_line(screenX + 1, screenY + 2, screenX + 3, screenY + 2);
    tgi_line(screenX + 4, screenY + 1, screenX + 4, screenY + 4);
    tgi_setpixel(screenX, screenY + 1);
}


void drawScratch(tPos x, tPos y, tScratchValues scratch)
{
    int screenX = screenSquaresX[x] + 1;
    int screenY = screenSquaresY[y] + 1;
    
    if (SCRATCH_TEST(scratch, 1)) {
        drawOne(screenX, screenY);
    }
    
    screenX += SCRATCH_WIDTH;
    if (SCRATCH_TEST(scratch, 2)) {
        drawTwo(screenX, screenY);
    }
    
    screenX += SCRATCH_WIDTH;
    if (SCRATCH_TEST(scratch, 3)) {
        drawThree(screenX, screenY);
    }
    
    screenX = screenSquaresX[x] + 1;
    screenY += SCRATCH_HEIGHT;
    if (SCRATCH_TEST(scratch, 4)) {
        drawFour(screenX, screenY);
    }
    
    screenX += SCRATCH_WIDTH;
    if (SCRATCH_TEST(scratch, 5)) {
        drawFive(screenX, screenY);
    }
    
    screenX += SCRATCH_WIDTH;
    if (SCRATCH_TEST(scratch, 6)) {
        drawSix(screenX, screenY);
    }
    
    screenX = screenSquaresX[x] + 1;
    screenY += SCRATCH_HEIGHT;
    if (SCRATCH_TEST(scratch, 7)) {
        drawSeven(screenX, screenY);
    }
    
    screenX += SCRATCH_WIDTH;
    if (SCRATCH_TEST(scratch, 8)) {
        drawEight(screenX, screenY);
    }
    
    screenX += SCRATCH_WIDTH;
    if (SCRATCH_TEST(scratch, 9)) {
        drawNine(screenX, screenY);
    }
    
    tgi_setcolor(COLOR_WHITE);
}


void updatePos(tPos x, tPos y, tSquareVal val, tScratchValues scratch, bool correct, bool invalid, bool knownAtStart)
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
        
        if (knownAtStart) {
            tgi_line(screenX + TEXT_OFFSET_X + TEXT_UNDERLINE_OFFSET_X,
                     screenY + TEXT_OFFSET_Y + TEXT_UNDERLINE_OFFSET_Y,
                     screenX + TEXT_OFFSET_X + TEXT_UNDERLINE_OFFSET_X + TEXT_UNDERLINE_WIDTH,
                     screenY + TEXT_OFFSET_Y + TEXT_UNDERLINE_OFFSET_Y);
        }
        
        if ((gameOptions.showWrong) &&
            (!correct))
            tgi_line(screenX, edgeY, edgeX, screenY);
        
        if ((gameOptions.showInvalid) &&
            (invalid))
            tgi_line(screenX, edgeY, edgeX, screenY);
    } else if (scratch != 0) {
        drawScratch(x, y, scratch);
    }
    
    if ((cursorX == x) &&
        (cursorY == y)) {
        tgi_line(screenX, screenY, edgeX, screenY);
        tgi_lineto(edgeX, edgeY);
        tgi_lineto(screenX, edgeY);
        tgi_lineto(screenX, screenY);
    }
}


void youWon(void)
{
    int wonX = (TOTAL_WIDTH - WON_WIDTH) / 2;
    int wonY = (TOTAL_HEIGHT - WON_HEIGHT) / 2;
    char *line1 = "You solved it!";
    char *line2 = "Press any key";
    int textHeight = tgi_textheight(line1);
    
    tgi_setcolor(COLOR_BLACK);
    tgi_bar(wonX, wonY, wonX + WON_WIDTH, wonY + WON_HEIGHT);
    
    tgi_setcolor(COLOR_WHITE);
    tgi_outtextxy(wonX + 10, wonY + (textHeight * 2), line1);
    tgi_outtextxy(wonX + 7, wonY + (textHeight * 4), line2);
    
    cgetc();
}


bool playGame(void)
{
    initUI();
    
    cursorX = 0;
    cursorY = 0;
    
    drawGrid();
    
    startGame(DIFFICULTY_EASY, updatePos);
    
    while (true) {
        char ch;
        bool shouldNotBeep = true;
        bool shouldRefresh = false;
        
        if (isPuzzleSolved()) {
            youWon();
            return true;
        }
        
        ch = cgetc();
        
        switch (ch) {
            case 'h':
            case 'H':
                textMode();
                displayInstructions();
                graphicsMode();
                break;
                
            case 'o':
            case 'O':
                textMode();
                shouldRefresh = setOptions();
                graphicsMode();
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
                shouldNotBeep = setValueAtPos(cursorX, cursorY, ch - '0');
                break;
                
            case CH_F1:
            case '!':
                shouldNotBeep = toggleScratchValueAtPos(cursorX, cursorY, 1);
                break;
                
            case CH_F2:
            case '@':
                shouldNotBeep = toggleScratchValueAtPos(cursorX, cursorY, 2);
                break;
                
            case CH_F3:
            case '#':
                shouldNotBeep = toggleScratchValueAtPos(cursorX, cursorY, 3);
                break;
                
            case CH_F4:
            case '$':
                shouldNotBeep = toggleScratchValueAtPos(cursorX, cursorY, 4);
                break;
                
            case CH_F5:
            case '%':
                shouldNotBeep = toggleScratchValueAtPos(cursorX, cursorY, 5);
                break;
                
            case CH_F6:
            case '^':
                shouldNotBeep = toggleScratchValueAtPos(cursorX, cursorY, 6);
                break;
                
            case CH_F7:
            case '&':
                shouldNotBeep = toggleScratchValueAtPos(cursorX, cursorY, 7);
                break;
                
            case CH_F8:
            case '*':
                shouldNotBeep = toggleScratchValueAtPos(cursorX, cursorY, 8);
                break;
                
            case CH_F9:
            case '(':
                shouldNotBeep = toggleScratchValueAtPos(cursorX, cursorY, 9);
                break;
        }
        if (!shouldNotBeep) {
            printf("\007");
        }
        if (shouldRefresh)
            refreshAllPos();
    }
    
    return false;
}