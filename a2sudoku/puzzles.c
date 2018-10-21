//
//  puzzles.c
//  a2sudoku
//
//  Created by Jeremy Rand on 2015-07-15.
//  Copyright (c) 2015 Jeremy Rand. All rights reserved.
//


#include <conio.h>
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


// Globals

tPuzzle thePuzzle;


// Implementation


void fileReadProblem(char *filename)
{
    clrscr();
    asm volatile ("STA %w", 0xc051);
    printf("\n\nUnable to read file %s\nPress any key to quit", filename);
    cgetc();
    exit(0);
}


#undef LOAD_DEBUG
tPuzzle *getRandomPuzzle(tDifficulty difficulty)
{
    FILE *puzzleFile;
    char *filename;
    uint16_t numPuzzles;
    tPuzzleNum randomPuzzleNum;
    
    if (difficulty == DIFFICULTY_EASY)
        filename = "easy.puzzles";
    else if (difficulty == DIFFICULTY_MEDIUM)
        filename = "medium.puzzles";
    else
        filename = "hard.puzzles";
    
#ifdef LOAD_DEBUG
    printf("\n\nLoading from %s\n", filename);
#endif
    
    puzzleFile = fopen(filename, "rb");
    if (puzzleFile == NULL) {
        fileReadProblem(filename);
    }
    
    if (fread(&numPuzzles, sizeof(numPuzzles), 1, puzzleFile) != 1) {
        fileReadProblem(filename);
    }
    
#ifdef LOAD_DEBUG
    printf("Found %u puzzles\n", numPuzzles);
#endif
    
    randomPuzzleNum = (rand() % numPuzzles);
    
#ifdef LOAD_DEBUG
    printf("Loading the %uth puzzle\n", randomPuzzleNum);
#endif
    
#if 0 // It seems there is no implementation of lseek on Apple // in cc65
    if (fseek(puzzleFile, sizeof(thePuzzle) * randomPuzzleNum, SEEK_CUR) != 0) {
        fileReadProblem(filename);
    }
#endif
    
    do {
        if (fread(&thePuzzle, sizeof(thePuzzle), 1, puzzleFile) != 1) {
            fileReadProblem(filename);
        }
        randomPuzzleNum--;
    } while (randomPuzzleNum != 0);
    
    fclose(puzzleFile);
    
#ifdef LOAD_DEBUG
    printf("Loaded the puzzle\n    Press any key to continue\n");
    cgetc();
#endif
    
    return &thePuzzle;
}


tSquareVal getStartValueAtPos(tPuzzle *puzzle, tPos x, tPos y)
{
    return PUZZLE_START_VAL(PUZZLE_SQUARE(puzzle, x, y));
}


bool checkValueAtPos(tPuzzle *puzzle, tSquareVal val, tPos x, tPos y)
{
    return (PUZZLE_SOLVED_VAL(PUZZLE_SQUARE(puzzle, x, y)) == val);
}


void savePuzzle(tPuzzle *puzzle, FILE *saveFile)
{
    fwrite(puzzle, sizeof(*puzzle), 1, saveFile);
}


tPuzzle *loadPuzzle(FILE *saveFile)
{
    if (fread(&thePuzzle, sizeof(thePuzzle), 1, saveFile) != 1)
        return NULL;
    
    return &thePuzzle;
}
