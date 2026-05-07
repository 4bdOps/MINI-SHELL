# How our Mini Shell Works

## The Flow
1. main.c: Reads what the user types
2. parser.c (P1): Breaks it into {command, args}
3. executor.c (P2/P3/P4): Runs the command
4. Displays result

## The Files
- src/main.c: The loop that keeps running
- src/parser.c: P1 does this
- src/executor.c: P2/P3/P4 share this
- include/parser.h: The blueprint everyone uses
