//
//  game.c
//  a2sudoku
//
//  Created by Jeremy Rand on 2015-07-15.
//  Copyright (c) 2015 Jeremy Rand. All rights reserved.
//


#include <conio.h>
#include <stdio.h>
#include <string.h>

#include "game.h"
#include "puzzles.h"


// Macros
#define SQUARE_XY(x, y) (theGame.squares[((y) * BOARD_SIZE) + (x)])
#define PREV_SQUARE_XY(x, y) (theGame.prevSquares[((y) * BOARD_SIZE) + (x)])
#define SAVE_GAME_FILE "a2sudoku.game"


// Typedefs

typedef struct tGameSquare {
    tSquareVal value;
    tScratchValues scratchValues;
    bool knownAtStart;
    bool correct;
    bool invalid;
} tGameSquare;


typedef struct tGame {
    tGameSquare squares[BOARD_SIZE * BOARD_SIZE];
    tGameSquare prevSquares[BOARD_SIZE * BOARD_SIZE];
    struct tPuzzle *puzzle;
    tUpdatePosCallback callback;
    bool undoValid;
    time_t startTime;
} tGame;


// Globals

tGame theGame;


// Implementation

void refreshPos(tPos x, tPos y)
{
    tGameSquare *square;
    
    if (theGame.callback == NULL)
        return;
    
    square = &(SQUARE_XY(x, y));
    theGame.callback(x, y, square->value, square->scratchValues, square->correct, square->invalid, square->knownAtStart);
}


void restartGame(void)
{
    tPos x, y;
    tGameSquare *square;
    
    for (y = 0; y < BOARD_SIZE; y++) {
        for (x = 0; x < BOARD_SIZE; x++) {
            square = &(SQUARE_XY(x, y));
            
            if (square->knownAtStart)
                continue;
            
            if ((square->value == EMPTY_SQUARE) &&
                (square->scratchValues == 0))
                continue;
            
            square->value = EMPTY_SQUARE;
            square->scratchValues = 0;
            square->correct = false;
            square->invalid = false;
            refreshPos(x, y);
        }
    }
    
    theGame.undoValid = false;
    theGame.startTime = _systime();
}


void startGame(tDifficulty difficulty, tUpdatePosCallback callback)
{
    tPos x, y;
    
    theGame.puzzle = getRandomPuzzle(difficulty);
    theGame.callback = callback;
    memset(&(theGame.squares), 0, sizeof(theGame.squares));
    
    for (y = 0; y < BOARD_SIZE; y++) {
        for (x = 0; x < BOARD_SIZE; x++) {
            tSquareVal squareVal = getStartValueAtPos(theGame.puzzle, x, y);
            
            if (squareVal != EMPTY_SQUARE) {
                SQUARE_XY(x, y).value = squareVal;
                SQUARE_XY(x, y).knownAtStart = true;
                SQUARE_XY(x, y).correct = true;
            }
        }
    }
    
    theGame.startTime = _systime();
}


void saveGame(void)
{
    FILE *saveFile = fopen(SAVE_GAME_FILE, "wb");
    if (saveFile != NULL) {
        bool isValid = true;
        fwrite(&isValid, sizeof(isValid), 1, saveFile);
        
        // Change the start time into an elapsed time before saving...
        
        if (theGame.startTime != 0xffffffff) {
            theGame.startTime = _systime() - theGame.startTime;
        }
        fwrite(&theGame, sizeof(theGame), 1, saveFile);
        savePuzzle(theGame.puzzle, saveFile);
        fclose(saveFile);
    }
}


void deleteGame(void)
{
    // So, I tried using unlink() from unistd.h but it seems it
    // does nothing on an Apple // with cc65.  Instead, I will
    // just open the file for writing and close it again which
    // will leave it empty.  That way, there won't be a saved
    // game in the file.
    FILE *saveFile = fopen(SAVE_GAME_FILE, "wb");
    if (saveFile != NULL) {
        bool isValid = false;
        fwrite(&isValid, sizeof(isValid), 1, saveFile);
        fclose(saveFile);
    }
}


#undef LOAD_GAME_DEBUG
bool loadGame(tUpdatePosCallback callback)
{
    bool isValid = false;
    bool result = false;
    FILE *saveFile= fopen(SAVE_GAME_FILE, "rb");
    
    if (saveFile == NULL) {
#ifdef LOAD_GAME_DEBUG
        printf("Cannot open save game file\n");
        cgetc();
#endif
        return false;
    }
    
    if ((fread(&isValid, sizeof(isValid), 1, saveFile) != 1) ||
        (!isValid)) {
        fclose(saveFile);
#ifdef LOAD_GAME_DEBUG
        printf("Save is not valid\n");
        cgetc();
#endif
        return false;
    }
    
    if (fread(&theGame, sizeof(theGame), 1, saveFile) != 1) {
        fclose(saveFile);
        deleteGame();
#ifdef LOAD_GAME_DEBUG
        printf("Unable to read game from save\n");
        cgetc();
#endif
        return false;
    }
    
    theGame.callback = callback;
    
    // The saved start time is an elapsed time.  Convert it back into a start time relative to now.
    if (theGame.startTime != 0xffffffff) {
        theGame.startTime = _systime() - theGame.startTime;
    }
    
    theGame.puzzle = loadPuzzle(saveFile);
    
    fclose(saveFile);
    deleteGame();
    
    if (theGame.puzzle == NULL) {
#ifdef LOAD_GAME_DEBUG
        printf("Unable to read puzzle from save\n");
        cgetc();
#endif
        return false;
    }
    
    return true;
}


void refreshAllPos(void)
{
    tPos x, y;
    
    for (y = 0; y < BOARD_SIZE; y++) {
        for (x = 0; x < BOARD_SIZE; x++) {
            refreshPos(x, y);
        }
    }
}


bool isPuzzleSolved(void)
{
    tPos x, y;
    
    for (y = 0; y < BOARD_SIZE; y++) {
        for (x = 0; x < BOARD_SIZE; x++) {
            if (!(SQUARE_XY(x, y).correct))
                return false;
        }
    }
    return true;
}


time_t timeToSolve(void)
{
    if (theGame.startTime == 0xffffffff)
        return theGame.startTime;
    
    return _systime() - theGame.startTime;
}


bool isSquareInvalid(tPos col, tPos row)
{
    tSquareVal value = SQUARE_XY(col, row).value;
    tPos x, y;
    tPos subSquareXStart, subSquareXEnd;
    tPos subSquareYStart, subSquareYEnd;
    
    // Empty is always valid
    if (value == EMPTY_SQUARE)
        return false;
    
    for (y = 0; y < BOARD_SIZE; y++) {
        // If this value appears somewhere else in the same column, it is invalid
        if ((y != row) &&
            (value == SQUARE_XY(col, y).value))
            return true;
    }
            
    for (x = 0; x < BOARD_SIZE; x++) {
        // If this value appears somewhere else in the same row, it is invalid
        if ((x != col) &&
            (value == SQUARE_XY(x, row).value))
            return true;
    }
    
    // Need to find the sub-square for this position and check among them...
    subSquareXStart = ((col / SUBSQUARE_SIZE) * SUBSQUARE_SIZE);
    subSquareXEnd = subSquareXStart + SUBSQUARE_SIZE;
    
    subSquareYStart = ((row / SUBSQUARE_SIZE) * SUBSQUARE_SIZE);
    subSquareYEnd = subSquareYStart + SUBSQUARE_SIZE;
    for (y = subSquareYStart; y < subSquareYEnd; y++) {
        for (x = subSquareXStart; x < subSquareXEnd; x++) {
            if (x == col)
                continue;
            if (y == row)
                continue;
            
            if (value == SQUARE_XY(x, y).value)
                return true;
        }
    }
    
    // If we haven't seen this same value in the column, row or subsquare,
    // then it is not invalid (it is valid).
    return false;
}


void refreshNeighbours(tPos col, tPos row, tSquareVal newValue, tSquareVal oldValue)
{
    tPos x, y;
    tGameSquare *square = &(SQUARE_XY(col, row));
    bool newInvalid;
    bool checkInvalid = true;
    tPos subSquareXStart, subSquareXEnd;
    tPos subSquareYStart, subSquareYEnd;
    
    newInvalid = isSquareInvalid(col, row);
    
    if (newInvalid != square->invalid) {
        square->invalid = newInvalid;
        refreshPos(col, row);
    }
    
    // If the value was empty, and this square is not invalid,
    // then nothing more to check.  We couldn't be adding a
    // new invalid square nor fixing an existing invalid
    // square.
    if ((oldValue == EMPTY_SQUARE) &&
        (!newInvalid))
        checkInvalid = false;
    
    for (y = 0; y < BOARD_SIZE; y++) {
        if (y == row)
            continue;
        
        square = &(SQUARE_XY(col, y));
        if (square->knownAtStart)
            continue;
        if (checkInvalid) {
            newInvalid = isSquareInvalid(col, y);
            
            if (newInvalid != square->invalid) {
                square->invalid = newInvalid;
                refreshPos(col, y);
            }
        }
        
        if ((newValue != EMPTY_SQUARE) &&
            (SCRATCH_TEST(square->scratchValues, newValue))) {
            square->scratchValues ^= (0x1 << newValue);
            refreshPos(col, y);
        }
    }
    
    for (x = 0; x < BOARD_SIZE; x++) {
        if (x == col)
            continue;
        
        square = &(SQUARE_XY(x, row));
        if (square->knownAtStart)
            continue;
        
        if (checkInvalid) {
            newInvalid = isSquareInvalid(x, row);
            
            if (newInvalid != square->invalid) {
                square->invalid = newInvalid;
                refreshPos(x, row);
            }
        }
        
        if ((newValue != EMPTY_SQUARE) &&
            (SCRATCH_TEST(square->scratchValues, newValue))) {
            square->scratchValues ^= (0x1 << newValue);
            refreshPos(x, row);
        }
    }
    
    subSquareXStart = ((col / SUBSQUARE_SIZE) * SUBSQUARE_SIZE);
    subSquareXEnd = subSquareXStart + SUBSQUARE_SIZE;
    
    subSquareYStart = ((row / SUBSQUARE_SIZE) * SUBSQUARE_SIZE);
    subSquareYEnd = subSquareYStart + SUBSQUARE_SIZE;
    for (y = subSquareYStart; y < subSquareYEnd; y++) {
        for (x = subSquareXStart; x < subSquareXEnd; x++) {
            if (x == col)
                continue;
            if (y == row)
                continue;
            
            square = &(SQUARE_XY(x, y));
            if (square->knownAtStart)
                continue;
            
            if (checkInvalid) {
                newInvalid = isSquareInvalid(x, y);
                
                if (newInvalid != square->invalid) {
                    square->invalid = newInvalid;
                    refreshPos(x, y);
                }
            }
            
            if ((newValue != EMPTY_SQUARE) &&
                (SCRATCH_TEST(square->scratchValues, newValue))) {
                square->scratchValues ^= (0x1 << newValue);
                refreshPos(x, y);
            }
        }
    }
}


bool setValueAtPos(tPos x, tPos y, tSquareVal val)
{
    tGameSquare *square = &(SQUARE_XY(x, y));
    bool update = false;
    bool checkValues = false;
    bool correct;
    tSquareVal oldValue = EMPTY_SQUARE;
    
    if (square->knownAtStart) {
        return false;
    }
    
    theGame.undoValid = true;
    memcpy(theGame.prevSquares, theGame.squares, sizeof(theGame.squares));
    
    if (square->value != val) {
        oldValue = square->value;
        square->value = val;
        update = true;
        checkValues = true;
    }
    
    if (square->scratchValues != 0) {
        square->scratchValues = 0;
        update = true;
    }
    
    if (checkValues) {
        correct = checkValueAtPos(theGame.puzzle, val, x, y);
        if (square->correct != correct) {
            square->correct = correct;
            update = true;
        }
    }
    
    if (update)
        refreshPos(x,y);
    
    if (checkValues)
        refreshNeighbours(x, y, val, oldValue);
    
    return true;
}


bool toggleScratchValueAtPos(tPos x, tPos y, tSquareVal val)
{
    tGameSquare *square = &(SQUARE_XY(x, y));
    
    if (square->knownAtStart) {
        return false;
    }
    
    theGame.undoValid = true;
    memcpy(theGame.prevSquares, theGame.squares, sizeof(theGame.squares));
    
    square->scratchValues ^= (0x1 << val);
    refreshPos(x, y);
    
    return true;
}


bool undoLastMove(void)
{
    tGameSquare *square;
    tGameSquare *prevSquare;
    tPos x, y;
    
    if (!theGame.undoValid)
        return false;
    
    for (y = 0; y < BOARD_SIZE; y++) {
        for (x = 0; x < BOARD_SIZE; x++) {
            square = &(SQUARE_XY(x, y));
            
            if (square->knownAtStart)
                continue;
            
            prevSquare = &(PREV_SQUARE_XY(x, y));
            if ((square->value != prevSquare->value) ||
                (square->scratchValues != prevSquare->scratchValues) ||
                (square->invalid != prevSquare->invalid)) {
                memcpy(square, prevSquare, sizeof(*square));
                refreshPos(x, y);
            }
        }
    }
    
    theGame.undoValid = false;
    
    return true;
}