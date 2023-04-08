#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_ARGS 20

char** tokenize_cmd(char* cmd) {
    char** args = malloc(MAX_ARGS * sizeof(char*));
    char* arg = strtok(cmd, " ");
    int i = 0;
    while (arg != NULL && i < MAX_ARGS) {
        args[i++] = arg;
        arg = strtok(NULL, " ");
    }
    args[i] = NULL;
    return args;
}

int main() {
    char command[1024];
    fgets(command, 1024, stdin);
    command[strlen(command) - 1] = '\0';

    int num_pipes = 0;
    for (int i = 0; command[i] != '\0'; i++) {
        if (command[i] == '|') {
            num_pipes++;
        }
    }

    char** argv[num_pipes + 1];
    int i = 0;
    char* cmd = command;
    char* token;
    while ((token = strsep(&cmd, "|")) != NULL && i <= num_pipes) {
        argv[i++] = tokenize_cmd(token);
    }

    // Print out the arguments for each command
    for (i = 0; i <= num_pipes; i++) {
        printf("Command %d:\n", i+1);
        for (int j = 0; argv[i][j] != NULL; j++) {
            printf("%s ", argv[i][j]);
        }
        printf("\n");
    }

    // Free the allocated memory for the arguments
    for (i = 0; i <= num_pipes; i++) {
        free(argv[i]);
    }

    return 0;
}
