#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include "help_functions.h"
#define SUCCESS_EXIT 0
#define FAULT_EXIT -1
#define LEN 1000
#define DEVIDER '|'

void shell_loop();
char * read_commands();
void run_commands(char *,int);
char ** get_new_command(char *);
char ** sub_command(char *,int,int);
int check_line_on_deviders(char *);
int amount_of_deviders(char *);
void run_command(char * line);

int flag_ending = 0;
int flag_only = 0;

int main()
{
    shell_loop();
    return SUCCESS_EXIT;
}

void shell_loop()
{
    char * line;
    int argc;
    for(;;)
    {
        line = read_commands();
        flag_only = 0;
        if (empty_line(line) == 1)
        {
            free_input_line(line);
            continue;
        }
        argc = check_line_on_deviders(line);
        if (argc == FAULT_EXIT)
        {
            fprintf(stderr,"::Incorrect command in terminal\n");
            continue;
        }
        if (argc == 1)
        {
            flag_only = 1;
        }
        run_commands(line,argc);
        free_input_line(line);
        if (flag_ending == 1)
        {
            exit(SUCCESS_EXIT);
        }
    }
}

char * read_commands()
{
    int symbol;
    int bufsize = LEN;
    int position = 0;
    char * line = (char *)malloc(LEN * sizeof(char));
    if(!line)
    {
        fprintf(stderr,"::Error in alloc memory\n");
        exit(FAULT_EXIT);
    }
    fprintf(stdout,"$:");
    for(;;)
    {
        symbol = getchar();
        if (symbol == '\n' || symbol == EOF)
        {
            line[position] = '\0';
            if (symbol == EOF)
            {
                if (position == 0)
                {
                    free_input_line(line);
                    exit(SUCCESS_EXIT);
                }
                flag_ending = 1;
            }
            break;
        }
        if (position >= bufsize)
        {
            bufsize += LEN;
            line = realloc(line,bufsize * sizeof(char));
            if (!line)
            {
                fprintf(stderr,"Error in realloc memory\n");
                exit(FAULT_EXIT);
            }
        }
        line[position] = (char)symbol;
        position++;
    }
    return line;
}

void run_commands(char * line,int argc)
{
    int fd[2];
    pid_t pid1,pid2;
    char ** argv;
    char * next_line = line;
    int position = 0;
    while(argc != 0)
    {
        argv = get_new_command(next_line);
        skip_to_zero(next_line,&position);
        next_line += (position + 1);
        switch(argc)
        {
            case 1:
                if (flag_only == 1)
                {
                    if (strcmp(argv[0],"exit") == 0)
                    {
                        free_list_of_commands(argv);
                        free_input_line(line);
                        exit(SUCCESS_EXIT);
                    }
                    pid1 = fork();
                    if (pid1 < 0)
                    {
                        fprintf(stderr,"::Error in fork()\n");
                        return;
                    }
                    else if (pid1 == 0)
                    {
                        execvp(argv[0],argv);
                        fprintf(stderr,"::Error in exec()\n");
                        exit(1);
                    }
                    while (wait(NULL) != -1);
                    return;
                }
                dup2(fd[0],0);
                execvp(argv[0],argv);
                fprintf(stderr,"::Error in exec()\n");
                exit(1);
            default:
                if (pipe(fd) == -1)
                {
                    fprintf(stderr,"::Error in pipe()\n");
                    exit(FAULT_EXIT);
                }
                pid1 = fork();
                if (pid1 == 0)
                {
                    dup2(fd[0],0);
                    close(fd[0]);
                    close(fd[1]);
                    argc--;
                    run_commands(next_line,argc);
                    exit(0);
                }
                close(fd[0]);
                pid2 = fork();
                if (pid2 == 0)
                {
                    dup2(fd[1],1);
                    close(fd[1]);
                    execvp(argv[0],argv);
                    fprintf(stderr,"::Error in exec()\n");
                    exit(1);
                }
                close(fd[1]);
                while(wait(NULL) != -1);
                return;
        }
    }   
    exit(0);
}

char ** get_new_command(char * line)
{
    int start = 0;
    int end = 0;
    char ** command;
    skip_to_zero(line,&end);
    return sub_command(line,start,end);
}

char ** sub_command(char * line,int start,int end)
{
    char ** command;
    int counter_of_arg = 0;
    int counter_of_len = 0;
    command = (char **)malloc((end - start + 1) * sizeof(char *));
    if (!command)
    {
        return NULL;
    }
    command[counter_of_arg] = NULL;
    while (start != end)
    {
        skip_spaces(line,&start);
        command[counter_of_arg] = (char *)malloc(LEN * sizeof(char));
        if(!command[counter_of_arg])
        {
            free_list_of_commands(command);
            return NULL;
        }
        counter_of_len = 0;
        while(check_on_space_or_end(line[start]))
        {
            command[counter_of_arg][counter_of_len] = line[start];
            start++;
            counter_of_len++;
        }
        command[counter_of_arg][counter_of_len] = '\0';
        counter_of_arg++;
        command[counter_of_arg] = NULL;
        skip_spaces(line,&start);
    }
    return command;
}

int check_line_on_deviders(char * line)
{
    int position = 0;
    for(;;)
    {
        if (is_there_devider_first(line,position) == 1)
        {
            return FAULT_EXIT;
        }
        if (skip_to_devider(line,&position) == 1)
        {
            return amount_of_deviders(line);
        }
        position++;
        if (check_smth_after_devider(line,position) == 1)
        {
            return FAULT_EXIT;
        }
    }
}

int amount_of_deviders(char * line)
{
    int position = 0;
    int counter = 0;
    while (line[position] != '\0')
    {
        if (line[position] == DEVIDER)
        {
            line[position] = '\0';
            counter++;
        }
        position++;
    }
    return (++counter);
}
