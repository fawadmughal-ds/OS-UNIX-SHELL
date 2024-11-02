#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>

#define MAX_INPUT_SIZE 1024
#define MAX_ARG_SIZE 100
#define MAX_HISTORY_SIZE 10

char *history[MAX_HISTORY_SIZE];
int history_count = 0;

// Function to add command to history
void add_to_history(char *command) {
    if (history_count < MAX_HISTORY_SIZE) {
        history[history_count++] = strdup(command);
    } else {
        free(history[0]); // Free the oldest command
        for (int i = 1; i < MAX_HISTORY_SIZE; i++) {
            history[i - 1] = history[i]; // Shift commands up
        }
        history[MAX_HISTORY_SIZE - 1] = strdup(command); // Add new command
    }
}

// Function to display command history
void display_history() {
    for (int i = 0; i < history_count; i++) {
        printf("%d %s\n", i + 1, history[i]);
    }
}

// Function to execute a command
void execute_command(char *input) {
    char *args[MAX_ARG_SIZE];
    char *token;
    int i = 0;

    // Tokenize the input string
    token = strtok(input, " \n");
    while (token != NULL) {
        args[i++] = token;
        token = strtok(NULL, " \n");
    }
    args[i] = NULL; // Null-terminate the array of arguments

    if (i == 0) {
        return; // No command entered
    }

    // Check for history reference
    if (args[0][0] == '!') {
        int cmd_num;
        if (strcmp(args[0], "!-1") == 0) {
            cmd_num = history_count - 1; // Last command
        } else {
            cmd_num = atoi(&args[0][1]) - 1; // Convert to index
            if (cmd_num < 0 || cmd_num >= history_count) {
                fprintf(stderr, "No such command in history.\n");
                return;
            }
        }
        strcpy(input, history[cmd_num]); // Get the command from history
        printf("Repeating: %s\n", input);
        execute_command(input); // Execute the retrieved command
        return;
    }

    // Add command to history before executing
    add_to_history(input);

    pid_t pid = fork();
    if (pid == -1) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) { // Child process
        execvp(args[0], args); // Execute the command
        perror("Execution failed");
        exit(EXIT_FAILURE);
    } else { // Parent process
        wait(NULL); // Wait for child to finish
    }
}

void display_prompt() {
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    printf("PUCITshell@%s$ ", cwd);
}

int main() {
    char input[MAX_INPUT_SIZE];

    while (1) {
        display_prompt(); // Display the shell prompt

        // Read input from the user
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\nExiting shell...\n");
            break; // Exit on CTRL+D
        }

        if (strcmp(input, "history\n") == 0) {
            display_history(); // Show command history
            continue;
        }

        execute_command(input); // Execute the command
    }

    // Free allocated memory for history commands before exiting
    for (int i = 0; i < history_count; i++) {
        free(history[i]);
    }

    return 0;
}