/*
 * SQUAROT --  SQUAre ROTate is a puzzle game where you rotate sections of a 3x3 grid to order
 *             numbers from 1 to 9. Features beautiful ASCII art graphics,
 *             multiple difficulty levels, and intuitive gameplay mechanics.
 *
 * -----------------------------------------------------------------------
 *
 * Copyright (C) 2018-2025 Claudio GENIO 
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *  *  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *  *  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * -----------------------------------------------------------------------  
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>
#include <windows.h>
#include <ctype.h>

#define SQUAROT_VERSION "1.0.0"

/* constants */
#define BOARD_SIZE 9
#define DIGIT_ROWS 5
#define DIGIT_COLS 4
#define MAX_LEVEL 4
#define LINE_BUFFER_SIZE 300
#define INFO_BUFFER_SIZE 100
#define DISPLAY_LINES 21

/* Display constants */
static const char* const PADDING_LEFT = "  ";
static const char* const HDIVISOR_FILL = "---------";
static const char* const HDIVISOR_EMPTY = ".........";
static const char* const VDIVISOR_LEFT_FILL = "...|";
static const char* const VDIVISOR_RIGHT_FILL = "|...";
static const char* const VDIVISOR = "..*";

/* Enumerations */
typedef enum {
    BOX_A = 1,
    BOX_B,
    BOX_C,
    BOX_D
} BoxType;

typedef enum {
    COLOR_BLACK = 0,
    COLOR_DARKBLUE,
    COLOR_DARKGREEN,
    COLOR_DARKCYAN,
    COLOR_DARKRED,
    COLOR_DARKMAGENTA,
    COLOR_DARKYELLOW,
    COLOR_GRAY,
    COLOR_DARKGRAY,
    COLOR_BLUE,
    COLOR_GREEN,
    COLOR_CYAN,
    COLOR_RED,
    COLOR_MAGENTA,
    COLOR_YELLOW,
    COLOR_WHITE = 15
} ConsoleColor;

/* Game structure */
typedef struct {
    int board[BOARD_SIZE];
    int attempts;
    int level;
    int is_won;
} GameState;

/* ASCII representations of numbers  */
static const char* const DIGIT_PATTERNS[BOARD_SIZE][DIGIT_ROWS] = {
    /* Number 1 */
    {".##.", "..#.", "..#.", "..#.", "..#."},
    /* Number 2 */
    {"####", "...#", "####", "#...", "####"},
    /* Number 3 */
    {"####", "...#", "..##", "...#", "####"},
    /* Number 4 */
    {"#...", "#...", "#.#.", "####", "..#."},
    /* Number 5 */
    {"####", "#...", "####", "...#", "####"},
    /* Number 6 */
    {"####", "#...", "####", "#..#", "####"},
    /* Number 7 */
    {"####", "#..#", "...#", "...#", "...#"},
    /* Number 8 */
    {"####", "#..#", "####", "#..#", "####"},
    /* Number 9 */
    {"####", "#..#", "####", "...#", "####"}
};

/*
 * Mappings for clockwise rotations 
 */
static const int BOX_ROTATIONS[MAX_LEVEL][4][2] = {
    /* BOX_A (clockwise: 0->1->4->3->0) */
    {{0, 1}, {1, 4}, {4, 3}, {3, 0}},
    /* BOX_B (clockwise: 1->2->5->4->1) */
    {{1, 2}, {2, 5}, {5, 4}, {4, 1}},
    /* BOX_C (clockwise: 3->4->7->6->3) */
    {{3, 4}, {4, 7}, {7, 6}, {6, 3}},
    /* BOX_D (clockwise: 4->5->8->7->4) */
    {{4, 5}, {5, 8}, {8, 7}, {7, 4}}
};

/* Forward declarations */
static void displayBoardWithMode(const GameState* game, int showLevelSelect);

/*
* Set console color
*/
static inline void setConsoleColor(ConsoleColor foreground, ConsoleColor background) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    
    if (hConsole != INVALID_HANDLE_VALUE) {
        SetConsoleTextAttribute(hConsole, (background << 4) | foreground);
    }
}

/*
 * Get the ASCII pattern for a digit in a specific row 
 */
static inline const char* getDigitPattern(int digit, int row) {
    if (digit < 1 || digit > BOARD_SIZE || row < 0 || row >= DIGIT_ROWS) {
        return "....";
    }
     
    return DIGIT_PATTERNS[digit - 1][row];
}

/*
* Reset console color to default gray on black
*/
static inline void resetConsoleColor(void) {
    setConsoleColor(COLOR_GRAY, COLOR_BLACK);
}

/*
*
*/
static void initializeGame(GameState* game) {
    int i;
    for (i = 0; i < BOARD_SIZE; i++) {
        game->board[i] = i + 1;
    }

    game->attempts = 0;
    game->level = 0;
    game->is_won = 0;
}

/*
 * Victory check
 */
static int victoryCheck(const GameState* game) {
    int i;
    for (i = 0; i < BOARD_SIZE; i++) {
        if (game->board[i] != i + 1) {
            return 0;
        }
    }

    return 1;
}

/* 
 *  Rotation using lookup table
 */
static void rotateBox(GameState* game, BoxType box) {
    if (box < BOX_A || box > BOX_D) return;
    
    const int (*rotations)[2] = BOX_ROTATIONS[box - 1];
    int temp[4];
    int i;
    
    /* Save the original values */
    for (i = 0; i < 4; i++) {
        temp[i] = game->board[rotations[i][0]];
    }
    
    /* Apply the rotation */
    for (i = 0; i < 4; i++) {
        game->board[rotations[i][1]] = temp[i];
    }
}

/* 
 * Shuffle the board based on complexity
 */
static void shuffleBoard(GameState* game, int complexity) {
    srand((unsigned int)time(NULL));
    BoxType lastBox = 0;
    int i;
    
    for (i = 0; i < complexity; i++) {
        BoxType box;
        do {
            box = (BoxType)((rand() % MAX_LEVEL) + 1);
        } while (box == lastBox && complexity > 1);
        
        rotateBox(game, box);
        lastBox = box;
    }
}

/*
 * Display character handling 
 */
static void renderCharacter(char ch, int* flags) {
      
    switch (ch) {
        case '!': flags[0] = !flags[0]; return;
        case '&': flags[1] = !flags[1]; return;
        case '@': flags[2] = !flags[2]; return;
    }
    
    if (flags[0]) {
        setConsoleColor(COLOR_GREEN, COLOR_BLACK);
    } else if (flags[1]) {
        setConsoleColor(COLOR_DARKGREEN, COLOR_BLACK);
    } else if (flags[2]) {
        setConsoleColor(COLOR_BLACK, COLOR_WHITE);
    } else {
        switch (ch) {
            case '.':
                setConsoleColor(COLOR_BLACK, COLOR_BLACK);
                ch = ' ';
                break;
            case '#':
                setConsoleColor(COLOR_YELLOW, COLOR_YELLOW);
                break;
            case '-': case '|': case ' ':
                setConsoleColor(COLOR_RED, COLOR_BLACK);
                break;
            case 'A': case 'B': case 'C': case 'D':
                setConsoleColor(COLOR_GREEN, COLOR_BLACK);
                break;
            case '*':
                setConsoleColor(COLOR_DARKGREEN, COLOR_BLACK);
                printf(" || ");
                return;
        }
    }
    
    printf("%c", ch);
}

static void displayLine(const char* line) {
    int flags[3] = {0, 0, 0}; /* infoLight, infoDark, infoAlert */
    int i;
    for (i = 0; line[i] != '\0'; i++) {
        renderCharacter(line[i], flags);
    }

    printf("\n");
}

/*
 * Separator generation
 */
static void generateSeparator(char lines[3][INFO_BUFFER_SIZE], const char* box1, const char* box2) {
    snprintf(lines[0], INFO_BUFFER_SIZE, "%s%s%s    %s%s%s", 
             PADDING_LEFT, VDIVISOR_LEFT_FILL, HDIVISOR_EMPTY, HDIVISOR_EMPTY, VDIVISOR_RIGHT_FILL, VDIVISOR);
    snprintf(lines[1], INFO_BUFFER_SIZE, "%s%s....%s....    ....%s....%s%s", 
             PADDING_LEFT, VDIVISOR_LEFT_FILL, box1, box2, VDIVISOR_RIGHT_FILL, VDIVISOR);
    snprintf(lines[2], INFO_BUFFER_SIZE, "%s%s%s    %s%s%s", 
             PADDING_LEFT, VDIVISOR_LEFT_FILL, HDIVISOR_EMPTY, HDIVISOR_EMPTY, VDIVISOR_RIGHT_FILL, VDIVISOR);
}

/* 
 * Info panel with level selection support
 */
static void generateInfoPanel(char infoLines[DISPLAY_LINES][INFO_BUFFER_SIZE], const GameState* game, int showLevelSelect) {
    int i;
    /* Initialize all lines */
    for (i = 0; i < DISPLAY_LINES; i++) {
        strcpy(infoLines[i], " ");
    }
    
    /* Header */
    const char* header[] = { 
        "! _  _  , ,  _   _   _  ___!",
        "!(_ | | | | |_| |_) | |  | !",
        "!__)|_\\ |_| | | | \\ |_|  | !",
        "!--------------------------!",
        "! ver." SQUAROT_VERSION " !"
    };
    
    for (i = 0; i < 5; i++) {
        strcpy(infoLines[i], header[i]);
    }
    
    /* Footer */
    strcpy(infoLines[20], "&made with !<3! by Claudio Genio&");
    
    if (showLevelSelect) {
        /* Level selection mode */
        strcpy(infoLines[7], "!                          !");
        strcpy(infoLines[8], "!   SELECT DIFFICULTY:     !");
        strcpy(infoLines[9], "!                          !");
        strcpy(infoLines[10], "! [1] Easy (10 moves)      !");
        strcpy(infoLines[11], "! [2] Medium (20 moves)    !");
        strcpy(infoLines[12], "! [3] Hard (30 moves)      !");
        strcpy(infoLines[13], "! [4] Very Hard (40 moves) !");
        strcpy(infoLines[14], "!                          !");
        strcpy(infoLines[15], "! Press 1, 2, 3, or 4      !");
        strcpy(infoLines[16], "!                          !");
    } else if (game->is_won) {
        /* Victory message */
        strcpy(infoLines[11], "@+------------------------+@");
        strcpy(infoLines[12], "@|       You Win...       |@");
        strcpy(infoLines[13], "@<+------------------------+@");
        strcpy(infoLines[15], "! Press Y for new game     !");
        strcpy(infoLines[16], "! Press N to quit          !");
        strcpy(infoLines[17], " ");
    } else {
        /* Normal game mode - show attempts */
        snprintf(infoLines[9], INFO_BUFFER_SIZE, "!Attempts :%d!", game->attempts);
        strcpy(infoLines[11], "!                          !");
        strcpy(infoLines[12], "! Controls:                !");
        strcpy(infoLines[13], "! A - Rotate top-left      !");
        strcpy(infoLines[14], "! B - Rotate top-right     !");
        strcpy(infoLines[15], "! C - Rotate bottom-left   !");
        strcpy(infoLines[16], "! D - Rotate bottom-right  !");
        strcpy(infoLines[17], "! ESC - Exit game          !");
    }
}

/*
 * Main display with level selection support
 */
static void displayBoard(const GameState* game) {
    displayBoardWithMode(game, 0); /* Normal game mode */
}

static void displayBoardWithMode(const GameState* game, int showLevelSelect) {
    char lines[DISPLAY_LINES][LINE_BUFFER_SIZE];
    char separators[2][3][INFO_BUFFER_SIZE];
    char infoLines[DISPLAY_LINES][INFO_BUFFER_SIZE];
    int  section, row, i;
    
    /* Generate components */
    generateInfoPanel(infoLines, game, showLevelSelect);
    generateSeparator(separators[0], "A", "B");
    generateSeparator(separators[1], "C", "D");
    
    /* Build display by sections */
    const int sections[3][3] = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}};
    
    for (section = 0; section < 3; section++) {
        int baseRow = section * 8;
        
        /* 5 rows for each section */
        for (row = 0; row < DIGIT_ROWS; row++) {
            int lineIdx = baseRow + row;
            const char* divider = (row == 2) ? HDIVISOR_FILL : HDIVISOR_EMPTY;
            
            snprintf(lines[lineIdx], LINE_BUFFER_SIZE, "%s%s%s%s%s%s%s%s", 
                    PADDING_LEFT,
                    getDigitPattern(game->board[sections[section][0]], row), divider,
                    getDigitPattern(game->board[sections[section][1]], row), divider,
                    getDigitPattern(game->board[sections[section][2]], row), 
                    VDIVISOR, infoLines[lineIdx]);
        }
        
        /* Separators (except after the last section) */
        if (section < 2) {
            for (i = 0; i < 3; i++) {
                int sepIdx = baseRow + DIGIT_ROWS + i;
                snprintf(lines[sepIdx], LINE_BUFFER_SIZE, "%s%s", 
                        separators[section][i], infoLines[sepIdx]);
            }
        }
    }
    
    /* Final display */
    system("cls");
    printf("\n");
    
    for (i = 0; i < DISPLAY_LINES; i++) {
        displayLine(lines[i]);
    }
    
    resetConsoleColor();
}

/*
 * Integrated level selection within the game interface
 */
static int selectDifficultyLevel(void) {
    GameState tempGame;
    const int levelMultipliers[] = {10, 20, 30, 40};
    
    /* Initialize a temporary game state for display */
    initializeGame(&tempGame);
    
    while (1) {
        /* Show level selection in the right panel */
        displayBoardWithMode(&tempGame, 1); /* 1 = show level select */
        
        char choice = getch();
        
        if (choice >= '1' && choice <= '4') {
            return levelMultipliers[choice - '1'];
        }
        
        if (choice == 27) { /* ESC */
            exit(0);
        }
        
        /* Invalid input, continue loop */
    }
}

/*
 * Input handling 
 */
static BoxType getPlayerInput(void) {
    printf("\nPress A, B, C, D to rotate sections, ESC to exit: ");
    
    while (1) {
        char key = getch();
        
        if (key == 27) { /* ESC */
            exit(0);
        }
        
        key = toupper(key);
        
        switch (key) {
            case 'A': return BOX_A;
            case 'B': return BOX_B;
            case 'C': return BOX_C;
            case 'D': return BOX_D;
            default: continue; /* Invalid input, try again */
        }
    }
}

/*
 *  main game loop
 */
static void playGame(GameState* game) {
    while (!game->is_won) {
        displayBoard(game);
        
        BoxType playerMove = getPlayerInput();
        rotateBox(game, playerMove);
        game->attempts++;
        game->is_won = victoryCheck(game);
    }
    
    /* Show victory screen */
    displayBoard(game);
}

/*
 *  main 
 */
int main(void) {
    GameState game;
    
    while (1) {
        initializeGame(&game);
        
        /* Difficulty selection and setup */
        int complexity = selectDifficultyLevel();
        shuffleBoard(&game, complexity);
        
        /* Play */
        playGame(&game);
        
        /* Ask if continue */
        printf("\nAnother game? (Y/N): ");
        char choice = getch();
        choice = toupper(choice);
        
        if (choice != 'Y') {
            break;
        }
    }
    
    printf("\nThank you for playing SQUAROT!\n");
    return 0;
}