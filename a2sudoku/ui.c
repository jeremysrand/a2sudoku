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
#include <string.h>
#include <tgi.h>
#include <tgi/tgi-mode.h>

#include "game.h"
#include "ui.h"
#include "mouseWrapper.h"
#include "drivers/a2_hires_drv.h"


// Macros and defines
#define SQUARE_WIDTH 28
#define SQUARE_HEIGHT 19

#define THICK_LINE_WIDTH 4
#define THIN_LINE_WIDTH 2
#define THICK_LINE_HEIGHT 3
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


void setCursorPos(tPos newX, tPos newY)
{
    tPos oldX = cursorX;
    tPos oldY = cursorY;
    
    if (newX >= BOARD_SIZE)
        newX = BOARD_SIZE - 1;
    
    if (newY >= BOARD_SIZE)
        newY = BOARD_SIZE - 1;
    
    if ((cursorX == newX) &&
        (cursorY == newY))
        return;
    
    cursorX = newX;
    cursorY = newY;
    
    refreshPos(cursorX, cursorY);
    refreshPos(oldX, oldY);
    
    moveMouseToPos(cursorX, cursorY);
}


void initUI(void)
{
    static bool tgi_inited = false;
    
    if (tgi_inited)
        return;
    
    // Install drivers
    tgi_install(&a2_hires_drv);
    
    tgi_init();
    
    tgi_inited = true;
    
    initMouse(setCursorPos);
}


void loadOptions(void)
{
    static bool optionsLoaded = false;
    FILE *optionsFile;
    
    if (optionsLoaded)
        return;

    optionsFile = fopen("a2sudoku.opts", "rb");
    if (optionsFile != NULL) {
        fread(&gameOptions, sizeof(gameOptions), 1, optionsFile);
        fclose(optionsFile);
    }
    optionsLoaded = true;
}


void saveOptions(void)
{
    FILE *optionsFile;
    optionsFile = fopen("a2sudoku.opts", "wb");
    if (optionsFile != NULL) {
        fwrite(&gameOptions, sizeof(gameOptions), 1, optionsFile);
        fclose(optionsFile);
    }
}


void shutdownUI(void)
{
    // Uninstall drivers
    tgi_uninstall();
    shutdownMouse();
}


void textMode(void)
{
    clrscr();
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
    bool shouldSave = false;
    
    while (keepLooping) {
        clrscr();
        
        //               1111111111222222222233333333334
        //      1234567890123456789012345678901234567890
        printf(
               "           Apple // Sudoku\n"
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
                shouldSave = true;
                break;
                
            case 'i':
            case 'I':
                gameOptions.showInvalid = !gameOptions.showInvalid;
                shouldSave = true;
                shouldUpdate = true;
                break;
                
            case 'w':
            case 'W':
                gameOptions.showWrong = !gameOptions.showWrong;
                shouldSave = true;
                shouldUpdate = true;
                break;
                
            default:
                keepLooping = false;
                break;
        }
    }
    
    if (shouldSave) {
        printf("\n\nSaving options...");
        saveOptions();
    }
    
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
           "           Apple // Sudoku\n"
           "            By Jeremy Rand\n"
           "\n"
           "The goal is to get the numbers from 1 to"
           "9 uniquely in each column, row and 3x3\n"
           "subsquare.  Move the cursor with arrow\n"
           "keys, IJKM or mouse.  Press a number key"
           "to enter a value.  Press a number key\n"
           "while holding shift or open apple to\n"
           "toggle a scratch value.  Press 0 to\n"
           "clear a square.  Play ends when the\n"
           "puzzle is solved.\n"
           "    Difficulty          : %s\n"
           "    Show invalid values : %s\n"
           "    Show wrong values   : %s\n"
           "\n"
           "Press escape or Q to quit at any time.\n"
           "Press O to change options.\n"
           "Press R to restart the current game.\n"
           "Press N to start a new game.\n"
           "Press H to see this info again.\n"
           "Press U to undo your last move.\n"
           "  Press O to change options now or any\n"
           "           other key to start",
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
    int line1Width;
    int line2Width;
    int wonWidth;
    int wonHeight;
    int wonX;
    int wonY;
    static char line1[80];
    char *line2 = "Press any key";
    time_t solutionTime = timeToSolve();
    int textHeight = tgi_textheight(line1);
    
    if (solutionTime == 0xffffffff) {
        snprintf(line1, sizeof(line1), "You solved it!");
    } else {
        snprintf(line1, sizeof(line1), "You solved it in %lu minutes!", solutionTime / 60);
    }
    
    line1Width = strlen(line1) * 6;
    line2Width = strlen(line2) * 6;
    
    wonHeight = textHeight * 7;
    wonWidth = line1Width;
    if (wonWidth < line2Width)
        wonWidth = line2Width;
    wonWidth += 20;
    
    wonX = (TOTAL_WIDTH - wonWidth) / 2;
    wonY = (TOTAL_HEIGHT - wonHeight) / 2;
    
    tgi_setcolor(COLOR_BLACK);
    tgi_bar(wonX, wonY, wonX + wonWidth, wonY + wonHeight);
    
    tgi_setcolor(COLOR_WHITE);
    tgi_outtextxy((TOTAL_WIDTH - line1Width) / 2, wonY + (textHeight * 2), line1);
    tgi_outtextxy((TOTAL_WIDTH - line2Width) / 2, wonY + (textHeight * 4), line2);
    
    cgetc();
}


bool playGame(void)
{
    static bool firstGame = true;
    bool shouldSave = false;
    bool gameLoaded = false;
    char ch;
    uint8_t *button0 = (uint8_t *)0xc061; // Address of button #0 or open apple
    
    initUI();
    
    textMode();
    if (firstGame) {
        firstGame = false;
        printf("\n\nChecking for a saved puzzle...");
        
        if (loadGame(updatePos)) {
            bool gotAnswer = false;
            
            printf("\n\nYou have a saved puzzle!\n    Would you like to continue it (Y/N)");
            
            while (!gotAnswer) {
                ch = cgetc();
                switch (ch) {
                    case 'y':
                    case 'Y':
                        printf("\n\nLoading your saved puzzle");
                        gameLoaded = true;
                        gotAnswer = true;
                        shouldSave = true;
                        break;
                        
                    case 'n':
                    case 'N':
                        gotAnswer = true;
                        break;
                        
                    default:
                        printf("\007");
                        break;
                }
            }
        }
    }
    
    cursorX = 0;
    cursorY = 0;
    
    if (!gameLoaded) {
        clrscr();
        
        printf("\n\nLoading a new puzzle for you\n    Please be patient...");
        startGame(gameOptions.difficulty, updatePos);
    }
    
    drawGrid();
    refreshAllPos();
    
    graphicsMode();
    
    while (true) {
        pollMouse();
        
        if (kbhit()) {
            bool shouldNotBeep = true;
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
                    if (setOptions())
                        refreshAllPos();
                    graphicsMode();
                    break;
                    
                case 'r':
                case 'R':
                    shouldSave = false;
                    restartGame();
                    break;
                    
                case 'n':
                case 'N':
                    return true;
                    
                case 'q':
                case 'Q':
                case CH_ESC:
                    shutdownUI();
                    if (shouldSave) {
                        clrscr();
                        printf("\n\nSaving your puzzle so you can continue\n    later...");
                        saveGame();
                    }
                    return false;
                    
                case 'i':
                case 'I':
                case 0x0B:  // Only defined with apple2enh targts, CH_CURS_UP:
                    if (cursorY != 0) {
                        setCursorPos(cursorX, cursorY - 1);
                    } else {
                        setCursorPos(cursorX, BOARD_SIZE - 1);
                    }
                    break;
                    
                case 'j':
                case 'J':
                case CH_CURS_LEFT:
                    if (cursorX != 0) {
                        setCursorPos(cursorX - 1, cursorY);
                    } else {
                        setCursorPos(BOARD_SIZE - 1, cursorY);
                    }
                    break;
                    
                case 'k':
                case 'K':
                case CH_CURS_RIGHT:
                    if (cursorX < BOARD_SIZE - 1) {
                        setCursorPos(cursorX + 1, cursorY);
                    } else {
                        setCursorPos(0, cursorY);
                    }
                    break;
                    
                case 'm':
                case 'M':
                case 0x0A:  // Only defined with apple2enh targts, CH_CURS_DOWN:
                    if (cursorY < BOARD_SIZE - 1) {
                        setCursorPos(cursorX, cursorY + 1);
                    } else {
                        setCursorPos(cursorX, 0);
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
                    if (*button0 > 127)
                        shouldNotBeep = toggleScratchValueAtPos(cursorX, cursorY, ch - '0');
                    else
                        shouldNotBeep = setValueAtPos(cursorX, cursorY, ch - '0');
                    if (shouldNotBeep)
                        shouldSave = true;
                    break;
                    
                case '!':
                    shouldNotBeep = toggleScratchValueAtPos(cursorX, cursorY, 1);
                    if (shouldNotBeep)
                        shouldSave = true;
                    break;
                    
                case '@':
                    shouldNotBeep = toggleScratchValueAtPos(cursorX, cursorY, 2);
                    if (shouldNotBeep)
                        shouldSave = true;
                    break;
                    
                case '#':
                    shouldNotBeep = toggleScratchValueAtPos(cursorX, cursorY, 3);
                    if (shouldNotBeep)
                        shouldSave = true;
                    break;
                    
                case '$':
                    shouldNotBeep = toggleScratchValueAtPos(cursorX, cursorY, 4);
                    if (shouldNotBeep)
                        shouldSave = true;
                    break;
                    
                case '%':
                    shouldNotBeep = toggleScratchValueAtPos(cursorX, cursorY, 5);
                    if (shouldNotBeep)
                        shouldSave = true;
                    break;
                    
                case '^':
                    shouldNotBeep = toggleScratchValueAtPos(cursorX, cursorY, 6);
                    if (shouldNotBeep)
                        shouldSave = true;
                    break;
                    
                case '&':
                    shouldNotBeep = toggleScratchValueAtPos(cursorX, cursorY, 7);
                    if (shouldNotBeep)
                        shouldSave = true;
                    break;
                    
                case '*':
                    shouldNotBeep = toggleScratchValueAtPos(cursorX, cursorY, 8);
                    if (shouldNotBeep)
                        shouldSave = true;
                    break;
                    
                case '(':
                    shouldNotBeep = toggleScratchValueAtPos(cursorX, cursorY, 9);
                    if (shouldNotBeep)
                        shouldSave = true;
                    break;
                    
                case 'u':
                case 'U':
                    shouldNotBeep = undoLastMove();
                    break;
                    
                default:
                    shouldNotBeep = false;
                    break;
            }
            
            if (!shouldNotBeep) {
                printf("\007");
            }
            
            if (isPuzzleSolved()) {
                youWon();
                return true;
            }
            
        }
    }
    
    return false;
}
