#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "stdio.h"
#include "errno.h"
#include "stdlib.h"
#include "unistd.h"
#include <string.h>

#define EQUAL 0
#define COMMAND_SIZE 10
#define COMMAND_LENGTH  10

int main() {

    char command[1024];
    char *token;
    char *outfile;
    int i, fd, amper, redirect, retid, status;
    char *argv[10];


    char path[256];
    int flag_prompt = 0;
    int was_command = 0;
    char *last_command = NULL;
    char *prompt = "hello";

    char command_list[COMMAND_SIZE][COMMAND_LENGTH]  ={"echo","cd","prompt","!!","quit"};


    while (1) {

        //get the current directory path of this project
        if (getcwd(path, sizeof path) != NULL) {
            printf("Current working directory is: %s \n", getcwd(path, sizeof path));
        } else {
            perror("getcwd()  error\n");
        }




        printf("%s: ",prompt);
        fgets(command, 1024, stdin);
        command[strlen(command) - 1] = '\0';



        /* parse command line */
        i = 0;
        token = strtok(command, " ");
        while (token != NULL) {
            argv[i] = token;
            token = strtok(NULL, " ");
            i++;
        }
        argv[i] = NULL;

        /* Is command empty */
        if (argv[0] == NULL)
            continue;
        /** ------------------------- My code ---------------------------- */
        //q1. --------- >> ----------------------
        if(strcmp(argv[i-2],">>")==EQUAL){


            for (int j = 0; j < COMMAND_SIZE; ++j) {
                if(strcmp(argv[0],command_list[j])==EQUAL){


                    break;
                }
                
            }
        }

        //q2. -------------------prompt--------------------
       else if (strcmp(argv[0], "prompt") == EQUAL) {
            if (strcmp(argv[1], "=") == EQUAL) {
                if (argv[2] != NULL) {
                    if (flag_prompt == 0) {
                        //free(prompt);
                        prompt = (char *) malloc(sizeof(argv[2]));
                        strcpy(prompt, argv[2]);
                        flag_prompt =1;
                    } else {
                        free(prompt);
                        prompt = (char *) malloc(sizeof(argv[2]));
                        strcpy(prompt, argv[2]);
                    }
                }
            }
            continue;
        }

        //q3. -------- echo-----------------
       else if (strcmp(argv[0], "echo") == EQUAL) {
            // todo null check

            if (strcmp(argv[i - 1], "$?") != EQUAL) {
                for (int j = 1; j < i; j = j + 1) {
                    printf("%s \n", argv[j]);
                }
            }
                // q4. -------- echo $? -----------------
            else {
                // todo idk if its work good
                printf("%d \n", WEXITSTATUS(status));
            }

        }
            // q5. -------------cd --------------------
            //changing  the position to command
            // chdir() is a system call witch presents in unistd header file that provides access to the POSIX* OPERATING SYSTEM API .
        else if (strcmp(argv[0], "cd") == EQUAL) {
            if (argv[1] != NULL) {
                chdir(argv[1]);
            }
        }

            //q6. ------------- !! ---------------------
        else if (strcmp(argv[0], "!!") == EQUAL) {
            if (last_command != NULL) {
                printf("%s \n", last_command);
            } else {
                printf("No commands yet.\n");
                continue;
            }
        }
            //q7. ----------- quit -------------
        else if (strcmp(argv[0], "quit") == EQUAL) {
            exit(0);
        }

            /** ------------------------- End  of my code ---------------------------- */


//        /* Does command line end with & */
//        if (!strcmp(argv[i - 1], "&")) {
//            amper = 1;
//            argv[i - 1] = NULL;
//        } else
//            amper = 0;
//
//        if (!strcmp(argv[i - 2], ">")) {
//            redirect = 1;
//            argv[i - 2] = NULL;
//            outfile = argv[i - 1];
//        } else{
//            redirect = 0;}

            /* for commands not part of the shell command language */

        else if (fork() == 0) {

            /* redirection of IO ? */
            if (redirect) {
                fd = creat(outfile, 0660);
                close(STDOUT_FILENO);
                dup(fd);
                close(fd);
                /* stdout is now redirected */
            }
            execvp(argv[0], argv);

        }

        if (last_command != NULL) {
            // clear the memory
            free(last_command);
        }
        was_command = 1;
        //store the last command
        last_command = (char *) malloc(sizeof(argv[0]));

        strcpy(last_command, argv[0]);

        /* parent continues here */
        if (amper == 0) {
            retid = wait(&status);
        }
    }
    if (last_command != NULL) {
        free(last_command);
    }
    if (prompt != NULL) {
        free(prompt);
    }

    return 0;
}
