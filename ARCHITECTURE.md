# Mini Shell Architecture

## 1. Overview

A simple Unix shell that can:
- Parse user commands
- Execute built-in commands (cd, ls, mkdir, pwd, echo, exit)
- Handle environment variables (setenv, unsetenv, env)
- Keep command history
- Handle pipes and redirection
- Execute script files

## 2. Execution Flow
### Detailed Flow:
1. `main.c` reads input from user
2. `parser.c` breaks input into tokens: {name, args}
3. `executor.c` checks if builtin or external
4. `builtin.c` executes built-in command, OR
5. External program is forked and executed
6. `history.c` saves the command
7. Result is displayed to user
8. Loop continues

## 3. File Organization
mini_shell/
├── Makefile              (Build system)
├── README.md             (Project overview)
├── ARCHITECTURE.md       (This file)
├── include/              (Header files)
│   ├── parser.h          (Command parsing)
│   ├── executor.h        (Command execution dispatcher)
│   ├── builtin.h         (Built-in commands)
│   ├── history.h         (Command history)
│   ├── error.h           (Error handling)
│   ├── redirect.h        (Redirection & pipes)
│   └── scripts.h         (Script execution)
├── src/                  (Implementation files)
│   ├── main.c            (Main loop)
│   ├── parser.c          (P1: Parsing logic)
│   ├── executor.c        (Execution dispatcher)
│   ├── builtin.c         (P2: Built-in implementations)
│   ├── history.c         (P3: History tracking)
│   ├── error.c           (P3: Error handling)
│   ├── redirect.c        (P4: Pipes & redirection)
│   ├── scripts.c         (P5: Script execution)
│   └── utils.c           (Common utilities)
└── tests/                (Test files)
└── test_parser.c     (Parser tests)
## 4. Module Responsibilities

### Parser (P1 - src/parser.c)
**Input:** User command string (e.g., "ls -la /tmp")
**Output:** t_command struct with name, args, argc
**Tasks:**
- Split input by spaces (handle quoted strings)
- Store arguments in array
- Mark redirections and pipes for other modules

**Key function:**
```c
t_command *parse_command_line(char *input);
```

### Built-in Commands (P2 - src/builtin.c)
**Input:** t_command struct
**Output:** Success/error code
**Tasks:**
- Implement: cd, ls, mkdir, pwd, echo, exit
- Implement: env, setenv, unsetenv
- Check if command is built-in

**Key functions:**
```c
int is_builtin(char *name);
int execute_builtin(t_command *cmd);
```

### History & Error Handling (P3)
**History (src/history.c):**
- Store commands in memory (FIFO list)
- Save to ~/.mini_shell_history on exit
- Load from file on startup

**Error (src/error.c):**
- Consistent error message format
- Handle system errors (errno)

### Redirection & Pipes (P4 - src/redirect.c)
**Input:** t_command with redirection info
**Output:** File descriptors set up correctly
**Tasks:**
- Detect < > >> in parser output
- Open/close files
- Use dup2() to redirect stdin/stdout
- Fork and pipe multiple commands

### Scripts (P5 - src/scripts.c)
**Input:** Filename (e.g., "myscript.sh")
**Output:** Results of all commands in file
**Tasks:**
- Read script file line by line
- Parse each line
- Execute each command

## 5. Data Structures

### t_command
```c
typedef struct s_command {
    char    *name;              // Command name
    char    **args;             // Arguments array
    int     argc;               // Argument count
    char    *input_file;        // For < redirection
    char    *output_file;       // For > redirection
    int     append_output;      // 1 for >>, 0 for >
    int     is_pipe_input;      // Reads from pipe
    int     is_pipe_output;     // Writes to pipe
} t_command;
```

## 6. Compilation & Running

### Build:
```bash
make
```

### Run:
```bash
./mini_shell
```

### Test:
```bash
make test
```

### Clean:
```bash
make fclean
```

## 7. Implementation Timeline

**Days 1-2:** Architecture & structure
**Days 3-7:** Core modules (parsing, built-ins, history)
**Days 8:** Mid-checkpoint review
**Days 9-13:** Advanced features (pipes, redirection)
**Days 14-15:** Testing, documentation, final cleanup

## 8. Coding Conventions

- **Style:** C89/C99
- **Indentation:** 4 spaces
- **Naming:** snake_case for functions, UPPER_CASE for macros
- **Types:** Use `typedef struct s_name { ... } t_name;`
- **Errors:** Return -1 for error, 0 for success (usually)
- **Memory:** Always free allocated memory!

## 9. Testing Strategy

Each module should be tested independently:
- Parser: Test with various command formats
- Built-ins: Test each command separately
- History: Verify save/load works
- Pipes: Test chaining multiple commands
- Scripts: Execute .sh files with multiple commands
