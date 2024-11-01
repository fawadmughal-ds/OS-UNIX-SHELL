#include "shell_v4.c"

// Execute built-in shell commands
void execute_builtin(char **args) {
    if (strcmp(args[0], "cd") == 0) {
        chdir(args[1]); // Change directory
    } else if (strcmp(args[0], "exit") == 0) {
        exit(0); // Exit shell
    } else if (strcmp(args[0], "help") == 0) {
        printf("Available commands: cd, exit, jobs, kill, help\n");
    }
}

