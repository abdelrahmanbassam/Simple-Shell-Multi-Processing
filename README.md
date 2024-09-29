# Simple Shell (Multi-Processing)

## Overview

This project is a simple Unix shell implemented in C for the Linux environment. The shell supports executing various commands both synchronously and asynchronously (in the background). It demonstrates basic Unix system calls like `fork()`, `execvp()`, and `waitpid()`, and integrates signal handling for managing background processes and avoiding zombie processes. It also includes built-in commands like `cd`, `echo`, and `export`, enabling variable handling and basic shell functionality.

## Features

1. **Command Execution:**
    - Execute commands without arguments (e.g., `ls`, `pwd`, `mkdir`).
    - Execute commands with arguments (e.g., `ls -l`, `rm -rf`).
    - Background command execution using `&` (e.g., `firefox &`).
    - Error handling for unknown commands.
2. **Built-in Commands:**
    - `cd` (change directory): Supports multiple use cases such as relative, absolute paths, `~`, and `..`.
    - `echo`: Print text or variables. Variables can be used in the format `echo "Hello $VAR"`.
    - `export`: Allows setting and expanding variables (e.g., `export x="hello"`).
3. **Signal Handling:**
    - Signal handling for `SIGCHLD` to manage background processes and log child process termination events.
    - Prevents zombie processes by reaping child processes when they terminate.
4. **Logging:**
    - A log file records whenever a child process terminates, ensuring tracking of background processes.

## System Requirements

- **Operating System**: Linux
- **Programming Language**: C
- **Development Environment**: Ubuntu (or any other Linux distribution)
- **Utilities Required**: A process monitor like `ksysguard` or `gnome-system-monitor` to visualize background processes.

## Compilation and Execution

To compile and run the simple shell:

1. **Compilation**:
    
    ```bash
    gcc -o simpleShell simpleShell.c
    ```
    
2. **Running**:
    
    ```bash
    bash
    ./simpleShell
    ```
    

## Shell Features

### 1. Command Execution (No Arguments)

- **Example**: `ls`, `pwd`
- **System Calls**: The shell uses `fork()` to create a new child process and `execvp()` to execute the command. The parent process waits for the child process to complete using `waitpid()`.

### 2. Command Execution (With Arguments)

- **Example**: `ls -l`, `mkdir test`
- **System Calls**: Similar to command execution without arguments, but the command is passed along with its arguments to `execvp()`.

### 3. Background Command Execution

- **Example**: `firefox &`
- **Handling**: For commands ending with `&`, the shell does not wait for the command to complete. The child process runs asynchronously, and the parent immediately returns control to the shell prompt.
- **Signal Handling**: The shell registers a signal handler for `SIGCHLD` to log when background processes terminate and to ensure there are no zombie processes left.

### 4. Built-in Commands

The shell supports three built-in commands:

### a. `cd` (Change Directory)

- **Usage**:
    - `cd` (returns to the home directory)
    - `cd ..` (goes to the parent directory)
    - `cd /absolute/path` (navigates to an absolute path)
    - `cd relative_path` (navigates to a relative path)

### b. `echo`

- **Usage**: Prints text or variables.
    - **Example**: `echo "Hello World"`, `echo "$USER"`
    - **Variable Expansion**: Variables are expanded if preceded by `$`. Example: `echo "Hello $x"`.

### c. `export`

- **Usage**: Exports variables for use in the shell.
    - **Example**:
        - `export x=5`
        - `export y="Hello World"`
        - `ls $x` (uses the variable `$x` as an argument for a command)

### 5. Logging and Signal Handling

- **Signal Handler**: A signal handler is registered using `sigaction()` to handle `SIGCHLD` signals. When a background child process terminates, the shell logs the termination to a text file.
- **Log File**: A basic text log file appends the line "Child process was terminated" each time a background process ends.

## Example Usage

Here is an example of how to interact with the shell and use its features:

1. **Executing Commands**:
    
    ```
    ls
    mkdir test
    ls -l
    ```
    
2. **Using `export` and Variable Expansion**:
    
    ```
    export x="-l -a -h"
    ls $x
    ```
    
3. **Running a Command in the Background**:
    
    ```
    firefox &
    ```
    
4. **Changing Directories**:
    
    ```
    cd ..
    cd /home/user/projects
    ```
    
5. **Exiting the Shell**:
    
    ```
    exit
    ```
    

## Handling Requests in the Shell

- **Command Parsing**: The shell parses the user input to identify the command and its arguments. If the command is a built-in function (`cd`, `echo`, or `export`), the shell directly handles it.
- **Command Execution**: If the command is an external program, the shell uses `fork()` to create a child process, then calls `execvp()` to execute the command in the child. The parent process waits for the child to finish (for foreground processes) or registers a `SIGCHLD` signal handler to manage background processes.
- **Signal Handling**: The shell handles `SIGCHLD` signals to log the termination of child processes and prevent zombies. The signal handler function is responsible for reaping the terminated child processes.
- **Background Execution**: For commands followed by `&`, the shell skips the waiting step, allowing it to execute other commands immediately.
