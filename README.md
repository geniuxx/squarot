SQUAROT
=======

SQUAROT (SQUAre ROTate) is a challenging puzzle game where you rotate sections of a 3x3 grid to arrange numbers from 1 to 9 in order.    

 ![squarot screen shot](www.colpodigenio.it/github/squarotc/screenshpt01.png)
 
## How to Play

The goal is simple: arrange the numbers 1 through 9 in sequential order on a 3x3 grid by rotating sections.

**Usage:** `squarot`

**Controls:**
- `A`: Rotate top-left section clockwise
- `B`: Rotate top-right section clockwise  
- `C`: Rotate bottom-left section clockwise
- `D`: Rotate bottom-right section clockwise
- `ESC`: Quit game
- `1-4`: Select difficulty level
- `Y/N`: Start new game / Exit

## Game Mechanics

Each section rotates 4 numbers in a 2x2 pattern:

```
Grid Layout:        Sections:
┌─────┬─────┬─────┐  ┌─────┬─────┐
│  1  │  2  │  3  │  │  A  │  B  │
├─────┼─────┼─────┤  ├─────┼─────┤
│  4  │  5  │  6  │  │  C  │  D  │
├─────┼─────┼─────┤  └─────┴─────┘
│  7  │  8  │  9  │
└─────┴─────┴─────┘
```

- **Section A** rotates positions: 1→2→5→4→1
- **Section B** rotates positions: 2→3→6→5→2  
- **Section C** rotates positions: 4→5→8→7→4
- **Section D** rotates positions: 5→6→9→8→5

## Difficulty Levels

1. **Easy**: 10 random moves (perfect for beginners)
2. **Medium**: 20 random moves (moderate challenge)
3. **Hard**: 30 random moves (for puzzle enthusiasts)
4. **Very Hard**: 40 random moves (expert level)
 

## Building

**Requirements:**
- GCC or compatible C compiler
- Windows: `windows.h` and `conio.h` (included with most Windows compilers)
- Linux/macOS: Terminal with VT100 escape sequence support

**Compile:**
```bash
gcc -o squarot squarot.c
```

**Run:**
```bash
./squarot
```
 
## License

SQUAROT was originally written in C# by **Claudio GENIO** (2018) and converted to C (2025). 

The project is released under the **BSD 2-Clause License**.
You can freely use, modify, and distribute it as long as you retain the original copyright notice and license. 
