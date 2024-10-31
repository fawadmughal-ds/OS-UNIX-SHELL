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
