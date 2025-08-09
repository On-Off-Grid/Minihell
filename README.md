# Minishell - A Bash-like Shell Implementation

A lightweight shell implementation in C that mimics basic bash functionality with command execution, piping, redirection, and built-in commands.

## 🚀 Quick Start

```bash
git clone <repository-url>
cd MINIHELL1.0
make
./minishell
```

## 📁 Project Structure

```
MINISHELL1.0/
├── Makefile                 # Build configuration
├── minishell.h             # Main header with structures and function prototypes
├── main.c                  # Entry point and main loop
├── lexer.c                 # Tokenization and lexical analysis
├── parser.c                # Syntax parsing and command structure building
├── exec.c                  # Command execution engine
├── builtins.c              # Built-in command implementations
├── env_utils.c             # Environment variable management
├── signals.c               # Signal handling (SIGINT, SIGQUIT)
├── error_handling.c        # Error management and reporting
├── utils.c                 # Utility functions and string operations
├── libft/                  # Custom C library
└── description.md          # Detailed project documentation
```

## 🏗️ Architecture Overview

### Core Data Structures

```c
typedef struct s_token {
    int type;                    // Token type (WORD, PIPE, REDIR_IN, etc.)
    char *value;                 // Token content
    struct s_token *next;        // Linked list pointer
} t_token;

typedef struct s_cmd {
    char **argv;                 // Command arguments array
    t_token *in;                // Input redirections list
    t_token *out;               // Output redirections list
    struct s_cmd *next;         // Next command in pipeline
} t_cmd;
```

### Execution Flow

1. **Lexical Analysis** (`lexer.c`) - Raw input → Token stream
2. **Syntax Parsing** (`parser.c`) - Token stream → Command structures
3. **AST Building** (`ast.c`) - Command structures → Execution tree
4. **Command Execution** (`exec.c`) - Process creation, pipes, redirections
5. **Built-ins/External** - Built-in dispatch or execve() calls
6. **Cleanup** - Memory deallocation and file descriptor management

## ⚡ Features Implemented

### Built-in Commands
- `cd [directory]` - Change working directory
- `pwd` - Print working directory
- `echo [args]` - Print arguments (supports -n flag)
- `env` - Display environment variables
- `export VAR=value` - Set environment variables
- `unset VAR` - Remove environment variables
- `exit [code]` - Exit shell with optional code

### Shell Features
- **Command Execution** - External programs via PATH resolution
- **Piping** - `cmd1 | cmd2 | cmd3` pipeline support
- **Redirections**:
  - Input: `cmd < file`
  - Output: `cmd > file`
  - Append: `cmd >> file`
  - Heredoc: `cmd << delimiter`
- **Environment Variables** - `$VAR` and `${VAR}` expansion
- **Signal Handling** - Ctrl+C (SIGINT) and Ctrl+\ (SIGQUIT)
- **Quote Handling** - Single and double quotes preserve spaces

### Advanced Features
- **Pipeline Execution** - Multi-command pipes with proper fd management
- **Error Handling** - Comprehensive error reporting with exit codes
- **Memory Management** - No memory leaks, proper cleanup
- **Signal Safety** - Different signal behavior for interactive/non-interactive modes

## 🔧 System Calls Used

| Category | System Calls | Purpose |
|----------|--------------|---------|
| Process Management | `fork()`, `execve()`, `wait()`, `waitpid()` | Process creation and synchronization |
| File Operations | `open()`, `close()`, `read()`, `write()`, `access()` | File I/O and permissions |
| File Descriptors | `dup()`, `dup2()`, `pipe()` | Redirection and piping |
| Directory | `chdir()`, `getcwd()` | Directory navigation |
| Signals | `signal()`, `sigaction()` | Signal handling setup |
| Memory | `malloc()`, `free()` | Dynamic memory management |

## 🧪 Testing Examples

```bash
# Basic commands
./minishell
$ echo "Hello World"
$ pwd
$ cd /tmp && pwd

# Redirections
$ echo "test" > output.txt
$ cat < output.txt
$ echo "append" >> output.txt

# Piping
$ ls -la | grep minishell
$ echo "hello world" | wc -w
$ cat /etc/passwd | head -5 | tail -2

# Environment variables
$ export TEST=value
$ echo $TEST
$ env | grep TEST

# Complex combinations
$ echo "line1" > test.txt && echo "line2" >> test.txt
$ cat test.txt | wc -l > count.txt && cat count.txt
```

## 🐛 For AI Debugging Assistance

### Key Functions to Analyze

1. **`exec_cmds()`** - Main execution controller (exec.c:15-45)
2. **`apply_redirections()`** - File descriptor manipulation (exec.c:120-180)
3. **`tokenize()`** - Lexical analysis entry point (lexer.c:30-80)
4. **`parse_tokens()`** - Syntax parsing logic (parser.c:25-90)

### Common Issue Areas

- **File Descriptor Leaks** - Check `close()` calls in redirection handling
- **Memory Leaks** - Verify `free()` calls for token/command structures  
- **Process Zombies** - Ensure `waitpid()` for all forked processes
- **Signal Race Conditions** - Signal handler interactions with main loop

### Debug Commands

```bash
# Build with debug flags
make debug

# Memory leak detection
valgrind --leak-check=full ./minishell

# System call tracing
strace -e trace=process,file ./minishell

# Core dump analysis
gdb ./minishell core
```

## 📝 Code Style & Conventions

- **Function Naming**: `verb_object()` pattern (e.g., `exec_cmd()`, `parse_tokens()`)
- **Variable Naming**: Snake case for locals, Hungarian for complex structures
- **Error Handling**: Return -1 for errors, set global errno when applicable
- **Memory Management**: Caller responsible for freeing returned pointers
- **File Descriptors**: Always backup and restore STDIN/STDOUT for built-ins

## 🔍 Implementation Notes

### Redirection Handling
File descriptors are manipulated using `dup2()` to redirect STDIN/STDOUT before command execution. Built-in commands backup original fds and restore them after execution.

### Pipeline Implementation  
Each command in a pipeline runs in a separate child process. Pipes are created with `pipe()` and connected using `dup2()`. Parent process waits for all children to complete.

### Environment Variables
Environment is passed as `char ***envp` to allow modification. Variable expansion happens during parsing phase before command execution.

### Signal Handling
Different signal behavior for interactive vs non-interactive modes. Child processes inherit modified signal handlers to prevent unwanted termination.

---

**Author**: [Your Name]  
**42 School Project** | **C Programming** | **System Programming**
