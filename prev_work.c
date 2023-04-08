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

#define CMD_LEN 10

char **tokenizer(char *command, int *argc)
{

    char **args = malloc(CMD_LEN * sizeof(char *));
    char *token = strtok(command, " ");
    int i = 0;

    while (token != NULL)
    { 
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;
    *argc = i;
    printf("from function = %s\n argc = %d ", command, *argc);
    return args;
}

int main()
{ 
    Stack *stack_commands = create_stack();

    char command[1024];
    char *token;
    int i;
    char *outfile;
    int fd, amper, redirect, filwrite, num_of_pipes, retid, status, argc1;
    int fildes[2];
    char *argv2[10];

    char path[256];
    int flag_prompt = 0;
    char *prompt = "hello";
    char str_status[10];

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

        if ((strcmp(command, "!!") == EQUAL))
        {
            if (stack_commands->size > 0)
            {
                strcpy(command, top(stack_commands));
                printf("%s\n", top(stack_commands));
            }
            else
            {
                continue;
            }
        }
        else
        {
            push(stack_commands, command);
        }

        /* check how many pipes in the command */
        num_of_pipes = 0;
        for (size_t i = 0; command[i] != '\0'; i++)
        {
            if (command[i] == '|')
            {
                num_of_pipes++;
            }
        }
        printf("num pipes -> %d \n", num_of_pipes);

        /* devide all the commands between pipes */
        char **argv[num_of_pipes + 1];
        int argc[num_of_pipes + 1];

        i = 0;
        char *cmd = command;
        
        while ((token = strsep(&cmd, "|")) != NULL)
        {
            argv[i] = tokenizer(token, &argc[i]);
            printf("token = %s\n",token);
            i++;
        }
        argv[i] = NULL;
            printf("token number %d -> %s --- argc : %d \n", i , token, argc[i]);
        printf("i = %d\n", i);
        /* print the pipe command to test ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~` */
        for (size_t j = 0; j < i; j++)
        {
            for (size_t k = 0; k < argc[j]; k++)
            {
                printf("%s ", argv[j][k]);
            }

            printf("| ");
            
            
        }
            printf("\n");
        break;
        

        // /* parse command line */
        // i = 0;
        // token = strtok(command, " ");
        // while (token != NULL)
        // {
        //     argv1[i] = token;
        //     token = strtok(NULL, " ");
        //     i++;
        //     if (token && !strcmp(token, "|"))
        //     {
        //         num_of_pipes = 1;
        //         break;
        //     }
        // }
        // argv[i] = NULL;
        // argc1 = i;

        // /* Does command contain pipe */
        // if (num_of_pipes)
        // {
        //     i = 0;
        //     while (token != NULL)
        //     {
        //         token = strtok(NULL, " ");
        //         argv2[i] = token;
        //         i++;
        //     }
        //     argv2[i] = NULL;
        // }

        /* Does command line end with & */
        if (!strcmp(argv[argc1 - 1], "&"))
        {
            amper = 1;
            argv[argc1 - 1] = NULL;
        }
        else
            amper = 0;

        // Checking all the redirect flags
        if (argc1 > 1 && !strcmp(argv[argc1 - 2], ">"))
        {
            redirect = 1;
            filwrite = 0;
            argv[argc1 - 2] = NULL;
            outfile = argv[argc1 - 1];
        }

        // q1. -------------------------------------------------
        else if (argc1 > 1 && !strcmp(argv[argc1 - 2], ">>"))
        {
            redirect = 1;
            filwrite = 1;

            argv[argc1 - 2] = NULL;
            outfile = argv[argc1 - 1];
        }
        else if (argc1 > 1 && !strcmp(argv[argc1 - 2], "2>"))
        {
            redirect = 2;
            filwrite = 0;

            argv[argc1 - 2] = NULL;
            outfile = argv[argc1 - 1];
        }
        else if (argc1 > 1 && !strcmp(argv[argc1 - 2], "2>>"))
        {
            redirect = 2;
            filwrite = 1;

            argv[argc1 - 2] = NULL;
            outfile = argv[argc1 - 1];
        }
        else
            redirect = 0;

        /* ------------------- READ -------------------- */
        if (argc1 > 1 && strcmp(argv[0], "read") == EQUAL)
        {
            char key[10];
            strcpy(key, argv[1]);
            fgets(command, 1024, stdin);
            command[strlen(command) - 1] = '\0';
            setenv(key, command, 1);

            continue;
        }

        // q2. -------------------prompt--------------------
        if (argc1 > 1 && strcmp(argv[1], "=") == EQUAL)
        {
            printf("found the =\n");
            if (strcmp(argv[0], "prompt") == EQUAL)
            {
                if (argv[2] != NULL)
                {
                    if (flag_prompt == 0)
                    {
                        // free(prompt);
                        prompt = (char *)malloc(sizeof(argv[2]));
                        strcpy(prompt, argv[2]);
                        flag_prompt = 1;
                    }
                    else
                    {
                        free(prompt);
                        prompt = (char *)malloc(sizeof(argv[2]));
                        strcpy(prompt, argv[2]);
                    }
                }
            }
            else if (argv[0][0] == VAR_TAG)
            {
                if (argv[2] != NULL)
                {

                    setenv(strdup(argv[0] + 1), argv[2], 1);
                }
            }

            continue;
        }
        // q3. ----------------------echo---------------------------------------------
        if (strcmp(argv[0], "echo") == EQUAL)
        {
            // todo null check
            // q4. --------------------status-----------------------------------------------
            if (strcmp(argv[i - 1], "$?") == EQUAL)
            {

                sprintf(str_status, "%d", WEXITSTATUS(status));
                strcpy(argv[i - 1], str_status);
            }
            else if (argv[1] != NULL)
            {
                strcpy(variable, argv[1]);

                if (variable[0] == VAR_TAG && argv[2] == NULL)
                {

                    char *value = getenv(strdup(variable + 1));
                    if (value != NULL)
                    {
                        printf("%s\n", value);
                    }
                }
                else
                {
                    for (size_t i = 1; i < argc1; i++)
                    {
                        printf("%s ", argv[i]);
                    }
                    printf("\n");
                }
            }
            continue;
        }

        // q5. ---------------------cd--------------------------------
        if (strcmp(argv[0], "cd") == EQUAL)
        {
            if (argv[1] != NULL)
            {
                chdir(argv[1]);
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

            if (num_of_pipes)
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
                    execvp(argv[0], argv);
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
                execvp(argv[0], argv);
            }
        }
        /* parent continues over here... */
        /* waits for child to exit if required */
        if (amper == 0)
            retid = wait(&status);
    }

    // operations at the end of the program.
    // if (prompt != NULL)
    // {
    //     free(prompt);
    // }

    destroy_stack(stack_commands);
    // destroyVarMap(vartbl);
}