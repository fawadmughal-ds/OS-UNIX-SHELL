#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <limits.h>

#define MAX_INPUT_SIZE 1024
#define MAX_ARG_SIZE 100

void handle_sigchld(int sig) {
    // Reap zombie processes
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

void display_prompt() {
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    printf("PUCITshell@%s$ ", cwd);
}

void execute_command(char *input) {
    char *args[MAX_ARG_SIZE];
    char *token;
    int i = 0;
    int background = 0; // Flag for background execution

    // Tokenize the input string
    token = strtok(input, " \n");
    while (token != NULL) {
        if (strcmp(token, "&") == 0) {
            background = 1; // Set background flag if '&' is found
            break;
        }
        args[i++] = token;
        token = strtok(NULL, " \n");
    }
    args[i] = NULL; // Null-terminate the array of arguments

    if (i == 0) {
        return; // No command entered
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) { // Child process
        execvp(args[0], args); // Execute the command
        perror("Execution failed");
        exit(EXIT_FAILURE);
    } else { // Parent process
        if (!background) {
            wait(NULL); // Wait for foreground process to finish
        } else {
            printf("[%d] %d\n", getpid(), pid); // Print background job info
        }
    }
}

int main() {
    char input[MAX_INPUT_SIZE];

    // Set up signal handler for SIGCHLD to handle zombie processes
    signal(SIGCHLD, handle_sigchld);

    while (1) {
        display_prompt(); // Display the shell prompt

        // Read input from the user
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\nExiting shell...\n");
            break; // Exit on CTRL+D
        }

        execute_command(input); // Execute the command
    }

    return 0;
}
