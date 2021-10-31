/*
 * customshell.c
 * -------------
 * Author - Banu Prasanth Pulicharla
 * Description - This shell will help one learn about processes and 
 * 				 basic process management in a shell.
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

//limits
#define MAX_TOKENS 100
#define MAX_STRING_LEN 100

size_t MAX_LINE_LEN = 10000;

// builtin commands
#define EXIT_STR "exit"
#define FORE_G "fg"
#define KILL_STR "kill"
#define LISTJOBS "listjobs"
#define EXIT_CMD 0
#define UNKNOWN_CMD 99
#define AMP_STR "&"
#define IN_STR "<"
#define OP_STR ">"
#define PIPE_STR "|"

FILE *fp; // file struct for stdin
char **tokens;
char *line;
int bg_present = 0;
char *cmd_list, *cmd_list2;
char *tmp_file;
int *pid_list;
int bg_counter = 0;
int pipe_count = 0;

// Intialization
void initialize()
{

    assert((line = malloc(sizeof(char) * MAX_STRING_LEN)) != NULL);

    // allocate space for individual tokens
    assert((tokens = malloc(sizeof(char *) * MAX_TOKENS)) != NULL);

    // open stdin as a file pointer
    assert((fp = fdopen(STDIN_FILENO, "r")) != NULL);

    assert((pid_list = malloc(sizeof(int *) * MAX_TOKENS)) != NULL);
    assert((cmd_list = malloc(sizeof(char *) * MAX_TOKENS)) != NULL);
}

// Logic to check IO Redirection
int check_redirection(char **tokens)
{
    for (int i = 0; tokens[i] != NULL; i++)
    {
        if (strcmp(tokens[i], IN_STR) == 0)
        {
            tmp_file = tokens[i + 1];
            tokens[i] = NULL;
            return 1;
        }
        else if (strcmp(tokens[i], OP_STR) == 0)
        {
            tmp_file = tokens[i + 1];
            tokens[i] = NULL;
            return 2;
        }
    }
    return 0;
}

void tokenize(char *string)
{
    int token_count = 0;
    int size = MAX_TOKENS;
    char *this_token;
    bg_present, pipe_count = 0;

    while ((this_token = strsep(&string, " \t\v\f\n\r")) != NULL)
    {

        if (strcmp(this_token, AMP_STR) == 0)
        {
            bg_present = 1;
            continue;
        }

        if (strcmp(this_token, PIPE_STR) == 0)
        {
            pipe_count++;
        }

        if (*this_token == '\0')
            continue;

        tokens[token_count] = this_token;

        token_count++;

        // if there are more tokens than space, reallocate more space
        if (token_count >= size)
        {
            size *= 2;

            assert((tokens = realloc(tokens, sizeof(char *) * size)) != NULL);
        }
    }
    tokens[token_count] = NULL; // execvp expects a NULL at the end
}

void read_command()
{
    assert(getline(&line, &MAX_LINE_LEN, fp) > -1);

    tokenize(line);
}

int run_command()
{
    pid_t pid;
    int status, stat, rd_flag;
    char test;
    int i = 0;
    char *this_cmd;

    if (tokens[0] == NULL) // Do nothing and show shell when no command is passed
    {
        return UNKNOWN_CMD;
    }
    else if (strcmp(tokens[0], EXIT_STR) == 0) // EXIT_STR = "exit"
    {
        return EXIT_CMD; // returning 0
    }
    else if (strcmp(tokens[0], FORE_G) == 0) // FORE_G = "fg"
    {
        int inp_pid = atoi(tokens[1]);
        waitpid(inp_pid, &status, 0);

        return UNKNOWN_CMD; // returning 0
    }
    else if (strcmp(tokens[0], KILL_STR) == 0) // KILL_STR = "kill"
    {
        kill(atoi(tokens[1]), SIGTERM); // Terminate PID
        sleep(1);
        waitpid(atoi(tokens[1]), &status, WNOHANG);

        return UNKNOWN_CMD; // returning 0
    }
    else if (strcmp(tokens[0], LISTJOBS) == 0)
    {
        if (bg_counter != 0)
        {
            assert((cmd_list2 = malloc(sizeof(char *) * MAX_TOKENS)) != NULL);
            printf("\033[0;35mList of backgrounded processes: (Count-%d)\n", bg_counter);
            printf("\033[0m");

            strcpy(cmd_list2, cmd_list);

            while ((this_cmd = strsep(&cmd_list2, ":")) != NULL)
            {
                int state = waitpid(pid_list[i], &status, WNOHANG);

                if (state == 0)
                {
                    printf("'%s' with PID %d => Status: RUNNING\n", this_cmd, pid_list[i]);
                }
                else
                {
                    printf("'%s' with PID %d => Status: FINISHED\n", this_cmd, pid_list[i]);
                }

                i++;
            }
        }
        else
        {
            printf("\033[0;34mNo Background Jobs Found!\n");
            printf("\033[0m");
        }
    }
    else if (1 == 1) // add logic at later stage to check if it is part of command list
    {

        if ((pid = fork()) < 0)
        {
            printf("\033[0;31mfork failed..\n");
            printf("\033[0m");
        }
        else if (pid == 0)
        {
            rd_flag = check_redirection(tokens);

            if (rd_flag == 1) // <
            {
                int fd_file = open(tmp_file, O_RDONLY);

                if (fd_file < 0)
                {
                    printf("\033[0;31mError Reading file '%s'.\n", tmp_file);
                    printf("\033[0m");
                    return EXIT_CMD;
                }
                else
                {
                    dup2(fd_file, 0);
                    close(fd_file);
                }
            }
            else if (rd_flag == 2) // >
            {
                int fd_file = open(tmp_file, O_CREAT | O_WRONLY | O_TRUNC, 600);

                if (fd_file < 0)
                {
                    printf("\033[0;31mError Creating/Writing file '%s'.\n", tmp_file);
                    printf("\033[0m");
                    return EXIT_CMD;
                }
                else
                {
                    dup2(fd_file, 1);
                    close(fd_file);
                }
            }
            execvp(tokens[0], tokens);
            return EXIT_CMD;
        }
        else
        {
            if (bg_present == 0) // No background
            {
                waitpid(pid, &status, 0);
            }
            else // Background present
            {

                bg_present = 0;
                pid_list[bg_counter] = pid;

                if (bg_counter != 0)
                {
                    strcat(cmd_list, ":");
                }
                strcat(cmd_list, tokens[0]);

                bg_counter++;
            }
            return UNKNOWN_CMD; // returning 99
        }
    }
    return UNKNOWN_CMD; // returning 99
}

// Below funtion is not complete.. not calling it anywhere!
void check_pipe() // Logic not complete.. timeup!! :(
{
    int fds[2];
    if (pipe_count != 0)
    {

        for (int i = 0; tokens[i] != NULL; i++)
        {
            // printf("%d # '%s'\n", i, tokens[i]);
            if (strcmp(tokens[i], PIPE_STR) == 0)
            {
                if (pipe(fds) == -1)
                {
                    perror("pipe");
                    exit(1);
                }
            }
            
        }
    }
}

void sign_handler(int signal){};

int main()
{
    initialize();
    signal(SIGINT, sign_handler);

    do
    {
        int strr = 1;
        printf("\033[0;32m***BS_OS550> "); //Added green color
        printf("\033[0m");                 //Resetting color
        read_command();
        // check_pipe();                      // Inprogress test logic

    } while (run_command() != EXIT_CMD);

    return 0;
}
