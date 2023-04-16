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
    // printf("from function = %s\n argc = %d ", command, *argc);
    return args;
}

int main()
{
    Stack *stack_commands = create_stack();

    char command[1024];
    char *token;
    int i;
    char *outfile;
    int fd, amper, redirect, filwrite, num_of_pipes, retid;
    int status = 0;

    char *argv2[10];

    char path[256];
    int flag_prompt = 0;
    char *prompt = "hello";
    char str_status[10];

    char variable[256];

    /* if statement check */
    int active_if = 0;
    int enter_if = 0;
    char *_then;
    char *_else;

    while (1)
    {
        if (!active_if)
        {
            printf("%s: ", prompt);
            fgets(command, 1024, stdin);
            command[strlen(command) - 1] = '\0';
        }
        else
        {
            if (!status)
            {
                strcpy(command, _then);
            }
            else
            {
                strcpy(command, _else);
            }
            free(_then);
            free(_else);
            active_if = 0;
        }

        if (strncmp(command, "if ", 3) == EQUAL)
            {
                strcpy(command, command + 3);
                enter_if = 1;
            }

        /* Is command empty */
        if (command[0] == '\0')
        {
            printf("Empty command\n");
            enter_if = 0;
            continue;
        }

        /* Exit command */
        if (strcmp(command, "quit") == EQUAL)
        {
            break;
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
            // printf("token = %s\n", token);
            i++;
        }
        argv[i] = NULL;
        // printf("token number %d -> %s --- argc : %d \n", i, token, argc[i]);
        // printf("i = %d\n", i);
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

        pid_t pid;
        int fildes[2];
        int prev_fd = 0;

        for (size_t i = 0; i <= num_of_pipes; i++)
        {
            /* =================================== All The Commands ===================================*/
            if (enter_if)
            {
                enter_if = 0;
                
                char tag[10];
                fgets(tag, 10, stdin);
                tag[strlen(tag) - 1] = '\0';
                if (strcmp(tag, "then") == EQUAL)
                {
                    _then = (char *)malloc(1024 * sizeof(char));
                    fgets(_then, 1024, stdin);
                    _then[strlen(_then) - 1] = '\0';
                }
                else
                {
                    printf("invalid syntax \n");
                    break;
                }

                fgets(tag, 10, stdin);
                tag[strlen(tag) - 1] = '\0';
                if (strcmp(tag, "else") == EQUAL)
                {
                    _else = (char *)malloc(1024 * sizeof(char));
                    fgets(_else, 1024, stdin);
                    _else[strlen(_else) - 1] = '\0';
                } else
                {
                    printf("invalid syntax \n");
                    free(_then);
                    break;
                }

                fgets(tag, 10, stdin);
                tag[strlen(tag) - 1] = '\0';
                if (strcmp(tag, "fi") != EQUAL)
                {
                    printf("not finished with fi \n");
                    free(_then);
                    free(_else);
                    break;
                }

                // printf("%s \n", _then);
                // printf("%s \n", _else);
                active_if = 1;
                
                /* removing the 'if ' syntax from the command to make it ready for execute */
                // char *substr = "if ";
                // printf("the command %s \n", argv[i][0]);
                // char *pointer_shift = strstr(argv[i][0], substr);
                // memmove(pointer_shift, pointer_shift + strlen(substr), strlen(pointer_shift + strlen(substr)) + 1);

            }

            /* ------------------- READ -------------------- */
            if (argc[i] > 1 && strcmp(argv[i][0], "read") == EQUAL)
            {
                char key[10];
                strcpy(key, argv[i][1]);
                fgets(command, 1024, stdin);
                command[strlen(command) - 1] = '\0';
                setenv(key, command, 1);

                continue;
            }

            // q2. -------------------prompt--------------------
            if (argc[i] > 1 && strcmp(argv[i][1], "=") == EQUAL)
            {
                printf("found the =\n");
                if (strcmp(argv[i][0], "prompt") == EQUAL)
                {
                    if (argv[i][2] != NULL)
                    {
                        if (flag_prompt == 0)
                        {
                            // free(prompt);
                            prompt = (char *)malloc(sizeof(argv[i][2]));
                            strcpy(prompt, argv[i][2]);
                            flag_prompt = 1;
                        }
                        else
                        {
                            free(prompt);
                            prompt = (char *)malloc(sizeof(argv[i][2]));
                            strcpy(prompt, argv[i][2]);
                        }
                    }
                }
                else if (argv[i][0][0] == VAR_TAG)
                {
                    if (argv[i][2] != NULL)
                    {

                        setenv(strdup(argv[i][0] + 1), argv[i][2], 1);
                    }
                }

                continue;
            }
            // q3. ----------------------echo---------------------------------------------
            if (strcmp(argv[i][0], "echo") == EQUAL)
            {
                // todo null check
                // q4. --------------------status-----------------------------------------------
                if (strcmp(argv[i][argc[i] - 1], "$?") == EQUAL)
                {
                    printf("Here called echo $? , status is %d \n", status);
                    sprintf(str_status, "%d", WEXITSTATUS(status));
                    strcpy(argv[i][argc[i] - 1], str_status);
                }
                else if (argv[i][1] != NULL)
                {
                    strcpy(variable, argv[i][1]);

                    if (variable[0] == VAR_TAG && argv[i][2] == NULL)
                    {

                        char *value = getenv(strdup(variable + 1));
                        if (value != NULL)
                        {
                            printf("%s\n", value);
                        }
                    }
                    else
                    {
                        for (size_t t = 1; t < argc[i]; t++)
                        {
                            printf("%s ", argv[i][t]);
                        }
                        printf("\n");
                    }
                }
                continue;
            }

            // q5. ---------------------cd--------------------------------
            if (strcmp(argv[i][0], "cd") == EQUAL)
            {
                if (argv[i][1] != NULL)
                {
                    chdir(argv[i][1]);
                }
                continue;
            }

            /* =================================== END OF ALL COMMANDS =================================== */

            // open pipe
            if (pipe(fildes) == -1)
            {
                perror("pipe");
                exit(EXIT_FAILURE);
            }
            /* ================== FORK ================== */
            pid = fork();

            if (pid == -1)
            {
                perror("fork");
                exit(EXIT_FAILURE);
            }

            if (pid == 0)
            {
                if (i < num_of_pipes)
                {
                    if (dup2(fildes[1], STDOUT_FILENO) == -1)
                    {
                        perror("dup2");
                        exit(EXIT_FAILURE);
                    }
                }
                if (i > 0)
                {
                    if (dup2(prev_fd, STDIN_FILENO) == -1)
                    {
                        perror("dup2");
                        exit(EXIT_FAILURE);
                    }
                    close(prev_fd);
                }

                /* Does command line end with & */
                if (!strcmp(argv[i][argc[i] - 1], "&"))
                {
                    amper = 1;
                    argv[i][argc[i] - 1] = NULL;
                }
                else
                    amper = 0;

                /* =================================== Redirect Check =================================== */

                // Checking all the redirect flags
                if (argc[i] > 1 && !strcmp(argv[i][argc[i] - 2], ">"))
                {
                    redirect = 1;
                    filwrite = 0;
                    argv[i][argc[i] - 2] = NULL;
                    outfile = argv[i][argc[i] - 1];
                }

                // q1. -------------------------------------------------
                else if (argc[i] > 1 && !strcmp(argv[i][argc[i] - 2], ">>"))
                {
                    redirect = 1;
                    filwrite = 1;

                    argv[i][argc[i] - 2] = NULL;
                    outfile = argv[i][argc[i] - 1];
                }
                else if (argc[i] > 1 && !strcmp(argv[i][argc[i] - 2], "2>"))
                {
                    redirect = 2;
                    filwrite = 0;

                    argv[i][argc[i] - 2] = NULL;
                    outfile = argv[i][argc[i] - 1];
                }
                else if (argc[i] > 1 && !strcmp(argv[i][argc[i] - 2], "2>>"))
                {
                    redirect = 2;
                    filwrite = 1;

                    argv[i][argc[i] - 2] = NULL;
                    outfile = argv[i][argc[i] - 1];
                }
                else
                    redirect = 0;

                /* =================================== END OF - Redirect Check =================================== */
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
                } /* stdout is now redirected */

                if (execvp(argv[i][0], argv[i]) == -1)
                {
                    perror("execvp");
                    exit(EXIT_FAILURE);
                }
                close(fildes[1]);
                exit(EXIT_SUCCESS);
            }
            close(fildes[1]);
            prev_fd = fildes[0];
        }

        for (size_t i = 0; i <= num_of_pipes; i++)
        {
            wait(&status);
        }

        /* for commands not part of the shell command language */

        // if (fork() == 0) // > >> 2> 2>>
        // {
        //     /* redirection of IO ? */
        //     if (redirect)
        //     {
        //         if (filwrite == 1)
        //         {
        //             fd = open(outfile, O_WRONLY | O_APPEND | O_CREAT, 0660);
        //             if (fd == -1)
        //             {
        //                 perror("open");
        //                 exit(EXIT_FAILURE);
        //             }
        //         }
        //         else
        //         {
        //             fd = creat(outfile, 0660);
        //         }

        //         if (redirect == 2)
        //         {
        //             close(STDERR_FILENO);
        //         }
        //         else
        //         {
        //             close(STDOUT_FILENO);
        //         }
        //         dup(fd);
        //         close(fd);
        //         /* stdout is now redirected */
        //     }

        //     if (num_of_pipes)
        //     {
        //         pipe(fildes);
        //         if (fork() == 0)
        //         {
        //             /* first component of command line */
        //             close(STDOUT_FILENO);
        //             dup(fildes[1]);
        //             close(fildes[1]);
        //             close(fildes[0]);
        //             /* stdout now goes to pipe */
        //             /* child process does command */
        //             execvp(argv[0], argv);
        //         }
        //         /* 2nd command component of command line */
        //         close(STDIN_FILENO);
        //         dup(fildes[0]);
        //         close(fildes[0]);
        //         close(fildes[1]);
        //         /* standard input now comes from pipe */
        //         execvp(argv2[0], argv2);
        //     }
        //     else
        //     {
        //         // printf("Got execvp\n");
        //         // printf("%s\n", argv1[0]);
        //         // printf("%s\n", argv1[1]);
        //         execvp(argv[0], argv);
        //     }
        // }
        // /* parent continues over here... */
        // /* waits for child to exit if required */
        // if (amper == 0)
        //     retid = wait(&status);

        // Free the allocated memory for the arguments
        for (i = 0; i <= num_of_pipes; i++)
        {
            free(argv[i]);
        }
    }

    // operations at the end of the program.
    // if (prompt != NULL)
    // {
    //     free(prompt);
    // }

    destroy_stack(stack_commands);
    // destroyVarMap(vartbl);
}