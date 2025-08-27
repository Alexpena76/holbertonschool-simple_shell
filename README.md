# HSH Simple Shell - Code Documentation

## Overview
This is a simple UNIX command interpreter implementation that provides basic shell functionality. The shell can execute commands, resolve paths, handle built-in commands, and manage processes using the fork-exec model. It operates in both interactive and non-interactive modes.

## Code Structure
The implementation consists of 15 main functions organized in a modular design:
main()
↓
shell_loop()
↓
├── read_input()
├── parse_input()
│   └── split_string()
├── execute_command()
│   ├── handle_builtin()
│   │   ├── builtin_exit()
│   │   └── builtin_env()
│   ├── find_command_in_path()
│   │   └── find_path_in_environ()
│   └── is_executable()
└── Memory Management
├── free_cmd()
├── free_array()
└── count_args()

## Function Documentation

### 1. `main(int argc, char *argv[])`
**Entry point** - Initializes shell and determines operating mode
**Parameters:**
- `argc`: Argument count (unused)
- `argv`: Argument vector (program name used for error messages)

**Return:** Always 0

**Key Operations:**
- Sets global program name
- Detects interactive mode using `isatty(STDIN_FILENO)`
- Launches main shell loop

### 2. `shell_loop(char *program_name)`
**Main execution loop** - Handles command processing cycle
**Parameters:**
- `program_name`: Shell program name for error messages

**Core Loop Logic:**
1. Display prompt (interactive mode only)
2. Read user input
3. Parse input into command structure
4. Execute command
5. Handle exit conditions and memory cleanup

**Exit Conditions:**
- EOF (Ctrl+D)
- `exit` built-in command
- Command not found in non-interactive mode

### 3. `read_input(void)`
**Input handler** - Reads and processes user input
**Return:** Allocated string containing user input, or NULL on EOF

**Algorithm:**
1. Use `getline()` to read complete line
2. Handle EOF gracefully
3. Strip trailing newline character
4. Return dynamically allocated string

**Memory Management:** Caller must free returned string

### 4. `parse_input(char *input)`
**Command parser** - Converts input string to command structure
**Parameters:**
- `input`: Raw input string from user

**Return:** Pointer to `cmd_t` structure, or NULL on error

**Parsing Process:**
1. Skip leading whitespace
2. Split input into tokens using `split_string()`
3. Set command name from first token
4. Count total arguments
5. Return populated command structure

### 5. `split_string(char *str, char *delim)`
**Tokenizer** - Splits string into array of tokens
**Parameters:**
- `str`: String to split
- `delim`: Delimiter characters (space and tab)

**Return:** NULL-terminated array of string tokens

**Algorithm:**
1. Allocate array for `MAX_ARGS` pointers
2. Create working copy of input string
3. Use `strtok()` to extract tokens
4. Create separate copies of each token with `strdup()`
5. NULL-terminate the array
