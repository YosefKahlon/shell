#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "stdio.h"
#include "errno.h"
#include "stdlib.h"
#include "unistd.h"
#include <string.h>
#include "stackCommands.c"
#include "var_table.c"

#define EQUAL 0
#define MAX_SIZE 50
#define VAR_TAG '$'

int main()
{
    Stack *stack_commands = create_stack();
    VarMap *vartbl = createVarMap(MAX_SIZE);

    char command[1024];
    char *token;
    int i;
    char *outfile;
    int fd, amper, redirect, filwrite, piping, retid, status, argc1;
    int fildes[2];
    char *argv1[10], *argv2[10];

    char path[256];
    int flag_prompt = 0;
    int was_command = 0;
    char *last_command = NULL;
    char *prompt = "hello";
    char str_status[10];

    int check_status = 0;
    char variable[256];

    while (1)
    {

        printf("%s: ", prompt);
        fgets(command, 1024, stdin);
        command[strlen(command) - 1] = '\0';

        /* Is command empty */
        if (command[0] == '\0')
        {
            printf("Empty command\n");
            continue;
        }

        /* Exit command */
        if (strcmp(command, "quit") == EQUAL)
        {
            exit(0);
        }

        /* Execute the last command */
/* Execute the last command */
        if ((strcmp(command, "!!") == EQUAL))
        {
            if (stack_commands->size > 0)
            {
                strcpy(command, top(stack_commands));
                printf("%s\n", top(stack_commands));
            }
        }
        else
        {
            push(stack_commands, command);
        }

        piping = 0;
        /* parse command line */
        i = 0;
        token = strtok(command, " ");
        while (token != NULL)
        {
            argv1[i] = token;
            token = strtok(NULL, " ");
            i++;
            if (token && !strcmp(token, "|"))
            {
                piping = 1;
                break;
            }
        }
        argv1[i] = NULL;
        argc1 = i;

        /* Does command contain pipe */
        if (piping)
        {
            i = 0;
            while (token != NULL)
            {
                token = strtok(NULL, " ");
                argv2[i] = token;
                i++;
            }
            argv2[i] = NULL;
        }

        /* Does command line end with & */
        if (!strcmp(argv1[argc1 - 1], "&"))
        {
            amper = 1;
            argv1[argc1 - 1] = NULL;
        }
        else
            amper = 0;

        // Checking all the redirect flags
        if (argc1 > 1 && !strcmp(argv1[argc1 - 2], ">"))
        {
            redirect = 1;
            filwrite = 0;
            argv1[argc1 - 2] = NULL;
            outfile = argv1[argc1 - 1];
        }

        //q1. -------------------------------------------------
        else if (argc1 > 1 && !strcmp(argv1[argc1 - 2], ">>"))
        {
            redirect = 1;
            filwrite = 1;

            argv1[argc1 - 2] = NULL;
            outfile = argv1[argc1 - 1];
        }
        else if (argc1 > 1 && !strcmp(argv1[argc1 - 2], "2>"))
        {
            redirect = 2;
            filwrite = 0;

            argv1[argc1 - 2] = NULL;
            outfile = argv1[argc1 - 1];
        }
        else if (argc1 > 1 && !strcmp(argv1[argc1 - 2], "2>>"))
        {
            redirect = 2;
            filwrite = 1;

            argv1[argc1 - 2] = NULL;
            outfile = argv1[argc1 - 1];
        }
        else
            redirect = 0;

        // q2. -------------------prompt--------------------
        if (strcmp(argv1[1], "=") == EQUAL)
        {
            printf("found the =\n");
            if (strcmp(argv1[0], "prompt") == EQUAL)
            {
                if (argv1[2] != NULL)
                {
                    if (flag_prompt == 0)
                    {
                        // free(prompt);
                        prompt = (char *)malloc(sizeof(argv1[2]));
                        strcpy(prompt, argv1[2]);
                        flag_prompt = 1;
                    }
                    else
                    {
                        free(prompt);
                        prompt = (char *)malloc(sizeof(argv1[2]));
                        strcpy(prompt, argv1[2]);
                    }
                }
            }
            else if (argv1[0][0] == VAR_TAG) /* save variables in the hashtable */
            {
                printf("We have var here\n");
                if (argv1[2] != NULL)
                {
                    put(vartbl, argv1[0], argv1[2]);
                    printf("this is the var we saved: ");
                    printf(" - %s\n",get(vartbl, argv1[0]));
                }
            }

            continue;
        }
        //q3. ----------------------echo---------------------------------------------
        if (strcmp(argv1[0], "echo") == EQUAL)
        {
            // todo null check
        //q4. --------------------status-----------------------------------------------
            if (strcmp(argv1[i - 1], "$?") == EQUAL)
            {
                check_status = 1;
                sprintf(str_status, "%d", WEXITSTATUS(status));
                strcpy(argv1[i - 1], str_status);
            } 
            else if (argv1[1] != NULL && argv1[2] == NULL)
            {
                printf("trying to assign var\n");
                strcpy(variable, argv1[1]);
                printf("successfuly assigned var\n");
                if (variable[0] == VAR_TAG)
                {
                    printf("trying to access map\n");
                    strcpy(argv1[1], get(vartbl, variable));
                }
                
            }
            
        }

        // q5. ---------------------cd--------------------------------
        if(strcmp(argv1[0], "cd") == EQUAL){
            if (argv1[1] != NULL) {
                chdir(argv1[1]);
            }
        }

        /* for commands not part of the shell command language */

        if (fork() == 0) // > >> 2> 2>>
        {
            /* redirection of IO ? */
            if (redirect)
            {
                if (filwrite == 1)
                {
                    fd = open(outfile, O_WRONLY | O_APPEND | O_CREAT, 0660);
                    if (fd == -1)
                    {
                        perror("open");
                        exit(EXIT_FAILURE);
                    }
                }
                else
                {
                    fd = creat(outfile, 0660);
                }

                if (redirect == 2)
                {
                    close(STDERR_FILENO);
                }
                else
                {
                    close(STDOUT_FILENO);
                }
                dup(fd);
                close(fd);
                /* stdout is now redirected */
            }

            if (piping)
            {
                pipe(fildes);
                if (fork() == 0)
                {
                    /* first component of command line */
                    close(STDOUT_FILENO);
                    dup(fildes[1]);
                    close(fildes[1]);
                    close(fildes[0]);
                    /* stdout now goes to pipe */
                    /* child process does command */
                    execvp(argv1[0], argv1);
                }
                /* 2nd command component of command line */
                close(STDIN_FILENO);
                dup(fildes[0]);
                close(fildes[0]);
                close(fildes[1]);
                /* standard input now comes from pipe */
                execvp(argv2[0], argv2);
            }
            else
            {
                // printf("Got execvp\n");
                // printf("%s\n", argv1[0]);
                // printf("%s\n", argv1[1]);
                execvp(argv1[0], argv1);
            }
        }
        /* parent continues over here... */
        /* waits for child to exit if required */
        if (amper == 0)
            retid = wait(&status);
    }

    // operations at the end of the program.
    if (prompt != NULL)
    {
        free(prompt);
    }

    destroy_stack(stack_commands);
    destroyVarMap(vartbl);
}