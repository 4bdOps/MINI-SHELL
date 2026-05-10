# P3 — History & Error Handling

## Files delivered

| File | Role |
|---|---|
| `history.h` | Struct `t_history` + function prototypes |
| `history.c` | Full implementation of the history module |
| `errors.h` | Enum `t_error` + `g_last_exit_code` + prototypes |
| `errors.c` | Full implementation of error handling and `$?` |

---

## Contract for P1 (Parser / Main loop)

### 1. Include the headers

```c
#include "history.h"
#include "errors.h"
```

### 2. Add `t_history` inside your shell struct

```c
typedef struct s_shell {
    char        *line;
    t_history   history;   // <-- add this
    int         last_exit;
} t_shell;
```

### 3. Call at startup

```c
history_init(&shell.history);
```

### 4. Call after each validated input line

```c
// After reading the user's input line and before executing:
history_add(&shell.history, shell.line);
```

### 5. Expand $? BEFORE parsing

```c
// In your variable expansion step, replace $? with:
char buf[12];
snprintf(buf, sizeof(buf), "%d", get_last_exit_code());
// then substitute buf in place of $? in the line
```

### 6. Set exit code AFTER each command execution

```c
// After waitpid() or after a built-in returns:
set_last_exit_code(exit_status);
```

### 7. Handle UP / DOWN arrows

```c
// When you detect ESC [ A (UP arrow):
history_prev(&shell.history);
char *cmd = history_current(&shell.history);
// cmd == NULL means clear the input line (like bash)

// When you detect ESC [ B (DOWN arrow):
history_next(&shell.history);
char *cmd = history_current(&shell.history);
```

### 8. Call at exit

```c
history_free(&shell.history);
```

---

## Contract for P2 (Built-in commands)

### Dispatch "history" built-in

```c
if (strcmp(cmd->name, "history") == 0)
    history_print(&shell.history);
```

### Use print_error() for your built-in errors

```c
// cd: directory not found
print_error(ERR_NO_SUCH_FILE, path);

// cd: permission denied
print_error(ERR_PERMISSION, path);

// cd: too many arguments
print_error(ERR_TOO_MANY_ARGS, "cd");

// exit: non-numeric argument
print_error(ERR_NUMERIC_ARG, arg);
```

---

## Contract for P4 (Pipes & Redirections)

```c
// After a pipe sequence, set the exit code
// $? must be the exit code of the LAST command in the pipe
set_last_exit_code(last_cmd_exit_status);

// If pipe() syscall fails:
print_error(ERR_PIPE_FAILED, NULL);

// If fork() fails:
print_error(ERR_FORK_FAILED, NULL);
```

---

## Contract for P5 (Scripts)

```c
// $? is already handled — just call get_last_exit_code()
// to evaluate conditions like: if [ $? -eq 0 ]
int status = get_last_exit_code();
```

---

## Exit codes reference

| Error | Exit code | Bash equivalent |
|---|---|---|
| Command not found | 127 | `foobar` |
| Permission denied | 126 | `./no_exec` |
| Syntax error | 2 | `ls |` |
| File not found | 1 | `cat x` |
| Pipe failed | 1 | internal |
| Fork failed | 1 | internal |
| Numeric arg required | 255 | `exit abc` |
| No error | 0 | `ls` |