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

void display_prompt() {
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    printf("PUCITshell@%s$ ", cwd);
}

void execute_command(char *input) {
    char *args[MAX_ARG_SIZE];
    char *token;
    int i = 0;
    int fd_in = -1, fd_out = -1;
    char *input_file = NULL;
    char *output_file = NULL;

    // Tokenize the input string
    token = strtok(input, " \n");
    while (token != NULL) {
        if (strcmp(token, "<") == 0) {
            input_file = strtok(NULL, " \n");
        } else if (strcmp(token, ">") == 0) {
            output_file = strtok(NULL, " \n");
        } else {
            args[i++] = token;
        }
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
        // Handle input redirection
        if (input_file) {
            fd_in = open(input_file, O_RDONLY);
            if (fd_in < 0) {
                perror("Input file open failed");
                exit(EXIT_FAILURE);
            }
            dup2(fd_in, STDIN_FILENO);
            close(fd_in);
        }

        // Handle output redirection
        if (output_file) {
            fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
            if (fd_out < 0) {
                perror("Output file open failed");
                exit(EXIT_FAILURE);
            }
            dup2(fd_out, STDOUT_FILENO);
            close(fd_out);
        }

        execvp(args[0], args); // Execute the command
        perror("Execution failed");
        exit(EXIT_FAILURE);
    } else { // Parent process
        wait(NULL); // Wait for child to finish
    }
}

void execute_pipe(char *input) {
    char *commands[MAX_ARG_SIZE];
    char *token;
    int i = 0;

    // Split the commands by pipe
    token = strtok(input, "|");
    while (token != NULL && i < MAX_ARG_SIZE - 1) {
        commands[i++] = token;
        token = strtok(NULL, "|");
    }
    commands[i] = NULL; // Null-terminate the array of commands

    int fd[2], prev_fd = -1;

    for (int j = 0; j < i; j++) {
        pipe(fd); // Create a pipe

        pid_t pid = fork();
        if (pid == -1) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        } else if (pid == 0) { // Child process
            if (prev_fd != -1) { // If not the first command
                dup2(prev_fd, STDIN_FILENO); // Get input from previous command
                close(prev_fd);
            }
            if (j < i - 1) { // If not the last command
                dup2(fd[1], STDOUT_FILENO); // Send output to next command
                close(fd[1]);
            }

            char *args[MAX_ARG_SIZE];
            int k = 0;
            char *sub_token = strtok(commands[j], " \n");

            while (sub_token != NULL && k < MAX_ARG_SIZE - 1) {
                args[k++] = sub_token;
                sub_token = strtok(NULL, " \n");
            }
            args[k] = NULL; // Null-terminate the array of arguments

            execvp(args[0], args); // Execute the command
            perror("Execution failed");
            exit(EXIT_FAILURE);
        } else { // Parent process
            close(fd[1]); // Close write end of the pipe
            if (prev_fd != -1) close(prev_fd); // Close previous read end of the pipe
            prev_fd = fd[0]; // Save read end of this pipe for next command
        }
    }

    wait(NULL); // Wait for all child processes to finish
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

        // Check for pipe in the input
        if (strchr(input, '|')) {
            execute_pipe(input); // Execute piped commands
        } else {
            execute_command(input); // Execute single command with redirection
        }
    }

    return 0;
}