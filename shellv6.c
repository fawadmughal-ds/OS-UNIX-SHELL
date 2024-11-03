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
#define MAX_VAR_SIZE 100
#define MAX_VAR_NAME_LENGTH 50

typedef struct {
    char name[MAX_VAR_NAME_LENGTH];
    char value[MAX_INPUT_SIZE];
    int global; // 1 for global (environment), 0 for local
} Variable;

Variable variables[MAX_VAR_SIZE];
int var_count = 0;

// Function to display the prompt
void display_prompt() {
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    printf("PUCITshell@%s$ ", cwd);
}

// Function to set a variable
void set_variable(char *name, char *value, int global) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            // Update existing variable
            strncpy(variables[i].value, value, MAX_INPUT_SIZE);
            variables[i].global = global;
            return;
        }
    }

    // Add new variable if not found
    if (var_count < MAX_VAR_SIZE) {
        strncpy(variables[var_count].name, name, MAX_VAR_NAME_LENGTH);
        strncpy(variables[var_count].value, value, MAX_INPUT_SIZE);
        variables[var_count].global = global;
        var_count++;
    } else {
        printf("Variable storage is full.\n");
    }
}

// Function to get a variable's value
char* get_variable(char *name) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            return variables[i].value;
        }
    }
    return NULL; // Variable not found
}

// Function to display all variables
void display_variables() {
    for (int i = 0; i < var_count; i++) {
        printf("%s=%s (%s)\n", variables[i].name, variables[i].value,
               variables[i].global ? "global" : "local");
    }
}

// Function to execute commands with variable support
void execute_command(char *input) {
    char *args[MAX_INPUT_SIZE];
    char *token;
    int i = 0;

    // Tokenize the input string
    token = strtok(input, " \n");
    while (token != NULL) {
        args[i++] = token;
        token = strtok(NULL, " \n");
    }
    args[i] = NULL; // Null-terminate the array of arguments

    if (i == 0) return; // No command entered

    // Check for variable assignment
    if (strchr(args[0], '=')) {
        char *var_name = strtok(args[0], "=");
        char *var_value = strtok(NULL, "=");
        
        if (var_value != NULL) {
            set_variable(var_name, var_value, 1); // Set as global by default
            return;
        }
    }

    // Check for retrieving a variable value
    if (args[0][0] == '$') {
        char *var_name = &args[0][1]; // Skip the '$'
        char *value = get_variable(var_name);
        
        if (value) {
            printf("%s\n", value); // Print the variable value
        } else {
            printf("Variable %s not found.\n", var_name);
        }
        return;
    }

    // Execute built-in command to list variables
    if (strcmp(args[0], "vars") == 0) {
        display_variables();
        return;
    }

    // Execute external commands
    pid_t pid = fork();
    if (pid == -1) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) { // Child process
        execvp(args[0], args); // Execute the command
        perror("Execution failed");
        exit(EXIT_FAILURE);
    } else { // Parent process
        wait(NULL); // Wait for child process to finish
    }
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

        execute_command(input); // Execute the command
    }

    return 0;
}