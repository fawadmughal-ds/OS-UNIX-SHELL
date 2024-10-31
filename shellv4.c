#include "shell_v3.c"

#define HISTORY_SIZE 10 // Maximum number of commands to store in history
char *history[HISTORY_SIZE]; // Array to store command history
int history_count = 0; // Count of commands in history

// Add command to history, with limit of HISTORY_SIZE
void add_to_history(char *input) {
    if (history_count < HISTORY_SIZE) {
        history[history_count++] = strdup(input);
    } else {
        free(history[0]);
        memmove(history, history + 1, (HISTORY_SIZE - 1) * sizeof(char *));
        history[HISTORY_SIZE - 1] = strdup(input);
    }
}
// Execute a command from history
void execute_history(char **args) {
    if (args[0][1] == '!') {
        int cmd_num = atoi(args[0] + 1) - 1; // Convert to integer, 1-based indexing
        if (cmd_num >= 0 && cmd_num < history_count) {
            strcpy(args[0], history[cmd_num]); // Replace with command from history
        }
    }
    execute_command_v3(args); // Execute command
}

