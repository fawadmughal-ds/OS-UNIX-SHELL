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

int main() {
    char input[MAX_CMD_LEN];
    char *args[MAX_ARGS];

    while (1) {
        display_prompt();
        if (fgets(input, MAX_CMD_LEN, stdin) == NULL) {
            printf("\nExiting shell...\n");
            break;
        }
        add_to_history(input); // Add command to history
        if (parse_command(input, args) > 0) {
            if (args[0][0] == '!') { // Check for history execution
                execute_history(args);
            } else {
                execute_command_v3(args);
            }
        }
    }
    return 0;
}
