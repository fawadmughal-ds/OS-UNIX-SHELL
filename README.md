# OS-UNIX-SHELL# MyShell Project

## Status
- **Current Status**: Completed
- **Bugs Found**: 
  - Occasionally, background jobs do not terminate properly if not managed correctly.
  - Variable retrieval using `$VAR` does not handle cases where the variable is not defined gracefully.
  - The `jobs` command may not update correctly if jobs are killed externally.

## Features Implemented
- **Built-in Commands**:
  - `cd [directory]`: Change the current working directory.
  - `exit`: Terminate the shell.
  - `jobs`: List all currently executing background processes with their process IDs.
  - `kill [job_number]`: Terminate a specified background process using its job number.
  - `help`: Display a list of available built-in commands and their usage.

- **User-defined Variables**:
  - Set variables using the syntax `VAR=value`.
  - Retrieve variable values using `$VAR`.
  - List all defined variables with the command `vars`.

- **Background Process Management**:
  - Run commands in the background by appending `&` to the command.
  - Display background jobs with their process IDs.

- **Command History (if implemented)**:
  - Repeat previous commands using `!number`, where `number` refers to the command's position in history.

## Additional Features
- Error checking for built-in commands and variable assignments.
- Clear and user-friendly prompts displaying the current working directory.
- Robust handling of invalid commands and inputs.

## Acknowledgments
- Special thanks to [Your Instructor's Name or Course Name] for guidance on shell programming concepts.
- Helpful resources:
  - [GeeksforGeeks](https://www.geeksforgeeks.org) for examples on process management and signal handling in C.
  - [The Linux Programming Interface](http://man7.org/tlpi/) by Michael Kerrisk for in-depth understanding of system calls and Unix programming.
  - Various online forums and communities for troubleshooting specific issues encountered during development.

