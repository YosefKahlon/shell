#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "stdio.h"
#include "errno.h"
#include "stdlib.h"
#include "unistd.h"
#include <string.h>
#include "stackCommands.c"

#define EQUAL 0
#define MAX_SIZE 50
#define VAR_TAG '$'

#define CMD_LEN 10

void sigint_handler(int sig)
{
    printf("\nYou typed Control-C!\n");
    fflush(stdout);
}

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
    return args;
}

int main()
{
    Stack *stack_commands = create_stack();

    signal(SIGINT, sigint_handler);

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
        int b = 0;
        int line_up = 0;
        int cmd_ptr = 0;
        memset(command, 0, 1024);
        if (!active_if)
        {
            int c = -1;
            printf("%s: ", prompt);

            while (!b)
            {
                c = getchar();
                if (c == '\n' && command != NULL)
                {
                    break;
                }
                if (c == '\033' && stack_commands->size > 0)
                {
                    // these two strange prints are from:
                    // https://itnext.io/overwrite-previously-printed-lines-4218a9563527
                    // and they delete the previous line

                    printf("\033[1A");
                    if (line_up)
                    {
                        printf("\x1b[2K"); // delete line
                        printf("\033[1A"); // line up
                        printf("\x1b[2K"); // delete line
                    }                      // line up
                    line_up = 1;
                    printf("\x1b[2K"); // delete line
                    printf("%s: ", prompt);
                    getchar(); // skip the [
                    switch (getchar())
                    { // the real value
                    case 'A':
                        // code for arrow up
                        if (cmd_ptr < get_stack_size(stack_commands))
                        {
                            cmd_ptr++;
                        }
                        strcpy(command, get_element_at(stack_commands, cmd_ptr));
                        printf("%s\n", command);

                        break;
                    case 'B':
                        // code for arrow down
                        if (cmd_ptr > 1)
                        {
                            strcpy(command, get_element_at(stack_commands, --cmd_ptr));
                            printf("%s\n", command);
                        }
                        else
                        {
                            memset(command, 0, 1024);
                            printf("\n");
                        }
                        break;
                    }

                    c = getchar();

                }
                else if (command != NULL)
                {
                    command[0] = c;
                    fgets(command + 1, 1023, stdin);

                    command[strlen(command) - 1] = '\0';
                    b = 1;
                }
            }
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
        if (command[0] == '\0' || command[0] == ('[' || '^'))
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
            i++;
        }
        argv[i] = NULL;

        // for (size_t j = 0; j < i; j++)
        // {
        //     for (size_t k = 0; k < argc[j]; k++)
        //     {
        //         printf("%s ", argv[j][k]);
        //     }

        //     printf("| ");
        // }
        // printf("\n");

        pid_t pid;
        int fildes[2];
        int prev_fd = 0;

        for (size_t i = 0; i <= num_of_pipes; i++)
        {
            /* Handling IF/ELSE query */
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
                }
                else
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

                active_if = 1;
            }

            /* =================================== All The Commands ===================================*/
            /* for commands not part of the shell command language */

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
                //                 else if (argc[i] > 1 && !strcmp(argv[i][argc[i] - 2], "<"))
                // {
                //     redirect = 3;
                //     filwrite = 0;
                //     argv[i][argc[i] - 2] = NULL;
                //     outfile = argv[i][argc[i] - 1];
                // }

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
            if (amper == 0)
            {
                wait(&status);
            }
        }

        

        // Free the allocated memory for the arguments
        for (i = 0; i <= num_of_pipes; i++)
        {
            free(argv[i]);
        }
    }

    // // operations at the end of the program.
    // if (prompt != NULL)
    // {
    //     free(prompt);
    // }

    destroy_stack(stack_commands);
}