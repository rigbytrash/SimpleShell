#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"


#define MAXARGS 32

void splitStringSemiColon(const char *input, char *first, char *second) {
    int i = 0;
    int j = 0;

    // initialise both output strings
    first[0] = '\0';
    second[0] = '\0';

    while (input[i] != '\0') {
        if (input[i] == ';') {
            // if a semicolon is detected, copy characters into the second string
            j = 0;
            i++;  // this is to skip the semicolon iteslf
            while (input[i] != '\0') {
                second[j] = input[i];
                i++;
                j++;
            }
            second[j] = '\0';
            break;
        } else {
            // copy characters into the first string
            first[j] = input[i];
            first[j+1] = '\0'; //  null-terminate first string
        }
        i++;
        j++;
    }
}



void amendString(char* a[], int index, const char* newString) {
    if (a[index] != 0) {
        // calculate the length of the original string and the new string
        int origLen = 0;
        while (a[index][origLen] != '\0') {
            origLen++;
        }
        int newLen = 0;
        while (newString[newLen] != '\0') {
            newLen++;
        }

        // allocate memory for the combined string
        // TODO google method to do this better
        char* combinedString = (char*)malloc(origLen + newLen + 1);

        if (combinedString != 0) {
            // copy the original string
            for (int i = 0; i < origLen; i++) {
                combinedString[i] = a[index][i];
            }

            // append the new string
            for (int i = 0; i < newLen; i++) {
                combinedString[origLen + i] = newString[i];
            }

            // null-terminate the combined string
            combinedString[origLen + newLen] = '\0';

            // update the array element to point to the new string
            a[index] = combinedString;
        }
    }
}

void splitStringByPipe(const char *input, char *splitArray[]) {
    int i = 0;
    int start = 0;

    for (int j = 0; input[j] != '\0'; j++) {
        if (input[j] == '|') {
            splitArray[i] = malloc(j - start + 1); // allocate  memory for the token
            if (splitArray[i] == 0) {
                fprintf(2, "Memory allocation failed\n");
                exit(1);
            }
            memmove(splitArray[i], input + start, j - start); // copy the token
            splitArray[i][j - start] = '\0'; // null-terminate the token
            i++;
            start = j + 1; // update the start position for the next token
        }
    }

    if (start < strlen(input)) {
        splitArray[i] = malloc(strlen(input) - start + 1); // allocate memory for the last token
        if (splitArray[i] == 0) {
            fprintf(2, "Memory allocation failed\n");
            exit(1);
        }
        memmove(splitArray[i], input + start, strlen(input) - start); // copy the last token
        splitArray[i][strlen(input) - start] = '\0'; // null-terminate the last token
        i++;
    }
}



void parse_command(char *cmd, char *argv[], int *argc, char **output_file, char **input_file) {
    *argc = 0;
    *output_file = 0;
    *input_file = 0;

    while (*cmd != '\0') {
        while (*cmd == ' ' || *cmd == '\t' || *cmd == '\n') {
            *cmd++ = '\0';
        }

        if (*cmd != '\0') {
            if (*cmd == '>') {
                *output_file = cmd + 1;
                while (*cmd != '\0' && *cmd != ' ' && *cmd != '\t' && *cmd != '\n' && *cmd != '>')
                    cmd++;
                if (*cmd == '>') {
                    *cmd++ = '\0';
                    while (*cmd == ' ' || *cmd == '\t' || *cmd == '\n'){
                        cmd++;
                    }
                }
            } else if (*cmd == '<') {
                *input_file = cmd + 1;
                while (*cmd != '\0' && *cmd != ' ' && *cmd != '\t' && *cmd != '\n' && *cmd != '<')
                    cmd++;
                if (*cmd == '<') {
                    *cmd++ = '\0';
                    while (*cmd == ' ' || *cmd == '\t' || *cmd == '\n'){
                        cmd++;
                    }
                }
            } else {
                argv[(*argc)++] = cmd;
            }
        }

        while (*cmd != '\0' && *cmd != ' ' && *cmd != '\t' && *cmd != '\n') {
            cmd++;
        }
    }

    argv[*argc] = 0;
}

void execute_command(const char* cmd) {

    char preinput[250]; // this is to check for ";"
    char second[128];
    splitStringSemiColon(cmd, preinput, second); // will put command into preinput and after ; in second


    char *argv[MAXARGS];
    int argc;
    // these two dont nec. need to be used
    char *output_file;
    char *input_file;

    parse_command((char*)preinput, argv, &argc, &output_file, &input_file);

    if (argc == 0) {
        fprintf(1, "No command provided.\n");
        return;
    }

    if (argc >= 1 && strcmp(argv[0], "cd") == 0) { // cd is a special case for some reason
        if (argc < 2) {
            fprintf(2, "cd: missing argument\n");
        } 
        else {
            if (chdir(argv[1]) < 0) {
                fprintf(2, "cd: cannot change directory to %s\n", argv[1]);
            }
        }
        return;
    }

    int pid = fork(); // create a child process to exec()
    if (pid < 0) {
        fprintf(1, "Fork failed\n");
    } else if (pid == 0) {
        // check if there's an input file redirection
        if (input_file) {
            *input_file = '\0';  // seperate the command and input file
            input_file++;  // move the pointer to the start of the input file name

            // open the input file and associate it with standard input (file descriptor 0)
            int input_fd = open(input_file, O_RDONLY);

            if (input_fd < 0) {
                fprintf(2, "Cannot open input file: %s\n", input_file);
                exit(1);
            }

            // redirect standard input to the input file descriptor
            close(0);  // close standard input
            dup(input_fd);  // duplicate the input file descriptor to standard input
            close(input_fd);  // close the input file descriptor
        }

        // check if there's an output file redirection
        if (output_file) {
            *output_file = '\0';  // seperate the command and output file
            output_file++;  // move the pointer to the start of the output file name

            // open the output file and save to a file descriptor
            int output_fd = open(output_file, O_CREATE|O_WRONLY);

            if (output_fd < 0) {
                fprintf(2, "Cannot open output file: %s\n", output_file);
                exit(1);
            }

            close(1);  // close standard output
            dup(output_fd);  // duplicate the output file descriptor to standard output
            close(output_fd);  // close the output file descriptor
        }

        exec(argv[0], argv);
        // if exec returns, there was an error
        fprintf(1, "my_shell: %s: command not found\n", argv[0]);
        exit(1);
    } else {
        wait(0);
    }

    if (second != 0 && second[0] != '\0') {
            execute_command(second);
    }
}


void execute_piped_commands(char *splitArray[], int pipeCount) {
    int pipes[2 * pipeCount]; // pipes are in/out next to one another
    int stdout_copy = dup(1); // stores the og fd for stdout 4 l8tr

    for (int i = 0; i < pipeCount; i++) { // create pipe pairs for each command
        if (pipe(pipes + 2 * i) < 0) {
            fprintf(2, "Pipe creation failed\n");
            exit(1);
        }
    }

    for (int i = 0; i < pipeCount + 1; i++) {
        int pid = fork(); // create child
        if (pid < 0) {
            fprintf(2, "Fork failed\n");
            exit(1);
        } else if (pid == 0) {
            if (i < pipeCount) { // intermediate commands are those that arent the last one
                // for intermediate commands, redirect standard output to the write end of the pipe
                close(1);
                dup(pipes[2 * i + 1]); // eg. i=0, the pipe is pipes[1]; i=1, the pipe is pipes[3]
            }
            if (i > 0) {
                // for intermediate and later commands, redirect standard input to the read end of the previous pipe
                close(0);
                dup(pipes[2 * (i - 1)]); // eg. i=1, the pipe is pipes[0]; i=2, the pipe is pipes[4]
            }
            for (int j = 0; j < 2 * pipeCount; j++) { // close ALL the pipes
                close(pipes[j]);
            }

            // check if it's the last command in the pipeline
            if (i == pipeCount) {
                // redirect standard output to the og stdout
                close(1);
                dup(stdout_copy); // duplicate standard output to the og
            }
            

            execute_command(splitArray[i]); // finally exec this command
            exit(0);
        }
    }


    for (int i = 0; i < 2 * pipeCount; i++) {
        close(pipes[i]);
    }

    for (int i = 0; i < pipeCount + 1; i++) {
        wait(0);
    }
}


int main(int argc, char *argv[]) {
    char cmd[250];

    while (1) { // keep running util break;
        char input[250]; // to pass to pipe splitter
        int pipeCount = -1;
        char *splitArray[MAXARGS];

        fprintf(1, ">>> ");
        gets(cmd, sizeof(cmd));


        if (cmd[0] == 0)
            continue;

        // copy cmd into input, and properly terminate
        strcpy(input, cmd);
        input[sizeof(input) - 1] = '\0';

        splitStringByPipe(input, splitArray); // each element contains a whole command

        for (int j = 0; splitArray[j] != 0; j++) {
            pipeCount++;
        }

        if (pipeCount > 0) {
            execute_piped_commands(splitArray, pipeCount);
        } 
        else {
            execute_command(splitArray[0]);
        }


        // free dynamically allocated memory in splitArray
        // V IMP, otherwise after each loop - the previous data is there and stdout doesnt fix
        for (int i = 0; i < 16; i++) {
            if (splitArray[i] != 0) {
                free(splitArray[i]);
                splitArray[i] = 0;
            }
        }
    }
    exit(0);
}
