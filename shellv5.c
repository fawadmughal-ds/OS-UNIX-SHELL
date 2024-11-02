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
#define MAX_JOBS 100

typedef struct {
    pid_t pid;
    char command[MAX_INPUT_SIZE];
} Job;

Job jobs[MAX_JOBS];
int job_count = 0;

// Function to display the prompt
void display_prompt() {
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    printf("PUCITshell@%s$ ", cwd);
}

// Function to change the current directory
void change_directory(char *path) {
    if (chdir(path) != 0) {
        perror("cd failed");
    }
}

// Function to add a job to the job list
void add_job(pid_t pid, char *command) {
    if (job_count < MAX_JOBS) {
        jobs[job_count].pid = pid;
        strncpy(jobs[job_count].command, command, MAX_INPUT_SIZE);
        job_count++;
    } else {
        printf("Job list is full. Cannot add more jobs.\n");
    }
}

// Function to display current jobs
void display_jobs() {
    for (int i = 0; i < job_count; i++) {
        printf("[%d] %d %s\n", i + 1, jobs[i].pid, jobs[i].command);
    }
}

// Function to kill a job by PID
void kill_job(int job_number) {
    if (job_number > 0 && job_number <= job_count) {
        pid_t pid = jobs[job_number - 1].pid;
        if (kill(pid, SIGKILL) == 0) {
            printf("Killed job [%d]: %d\n", job_number, pid);
            // Remove the job from the list
            for (int i = job_number - 1; i < job_count - 1; i++) {
                jobs[i] = jobs[i + 1];
            }
            job_count--;
        } else {
            perror("Failed to kill job");
        }
    } else {
        printf("No such job: %d\n", job_number);
    }
}

// Function to execute external commands or built-in commands
void execute_command(char *input) {
    char *args[MAX_ARG_SIZE];
    char *token;
    int i = 0;
    int background = 0;

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

    if (i == 0) return; // No command entered

    // Check for built-in commands
    if (strcmp(args[0], "cd") == 0) {
        if (i > 1) change_directory(args[1]);
        return;
    } else if (strcmp(args[0], "exit") == 0) {
        exit(0);
    } else if (strcmp(args[0], "jobs") == 0) {
        display_jobs();
        return;
    } else if (strcmp(args[0], "kill") == 0 && i > 1) {
        int job_number = atoi(args[1]);
        kill_job(job_number);
        return;
    } else if (strcmp(args[0], "help") == 0) {
        printf("Built-in commands:\n");
        printf("cd [directory] - Change working directory\n");
        printf("exit - Exit the shell\n");
        printf("jobs - List background jobs\n");
        printf("kill [job_number] - Kill a background process\n");
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
        add_job(pid, input); // Add to jobs list
        if (!background) wait(NULL); // Wait for foreground process to finish
        else printf("[%d] %d\n", job_count, pid); // Print background job info
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