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

int main() {
    char input[MAX_CMD_LEN];
    char *args[MAX_ARGS];

    while (1) {
        display_prompt();
        if (fgets(input, MAX_CMD_LEN, stdin) == NULL) {
            printf("\nExiting shell...\n");
            break;
        }
        add_to_history(input);
        if (parse_command(input, args) > 0) {
            if (args[0][0] == '!') {
                execute_history(args);
            } else if (strcmp(args[0], "cd") == 0 || strcmp(args[0], "exit") == 0 || strcmp(args[0], "help") == 0) {
                execute_builtin(args);
            } else {
                execute_command_v3(args);
            }
        }
    }
    return 0;
}
