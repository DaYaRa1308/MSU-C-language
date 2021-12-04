#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "help_little_functions.h"
#define SUCCESS_EXIT 0
#define FAULT_EXIT -1
#define LEN 1000
#define DEVIDER '|'

void shell_loop();
char * read_commands();
void run_commands(char *,int,char *);
char ** get_new_command(char *);
char ** sub_command(char *,int,int);
int amount_of_deviders(char *);
void run_command(char * line);
void go_to_directory(char **);
char * get_current_directory(void);
int not_execable(char *);
int check_line_on_stds(char *);
void restd(char *);
void redup(char *,int,int,char []);
int check(char *,int);

int flag_ending = 0;
int flag_only = 0;
char ** argv = NULL;
pid_t pid1,pid_n,pid_shell,pid3,pid4;
pid_t pid2 = 0;
int stdin_orig;
int stdout_orig;                                 
int flag_std = 0;
int flag_exit = 0;
int flag_continue = 0;
int flag_stp = 0;
int fd;

void Father_handler_one(int sig)
{
    if (sig == SIGUSR1)
    {
        go_to_directory(argv);
        return;
    }
    if (sig == SIGINT)
    {
        int counter = 0;
        fprintf(stdout,"::SIGINT ~ PID: %d\n",pid2);
        fprintf(stdout,"::List of arguements:\n");
        while (argv[counter] != NULL)
        {
            fprintf(stdout,"%s\n",argv[counter]);
            counter++;
        }
        flag_exit = 1;
        return;
    }
    if (sig == SIGTSTP)
    {
        int counter = 0;
        fprintf(stdout,"::SIGTSTP ~ PID: %d\n",pid2);
        fprintf(stdout,"::List of arguements:\n");
        while (argv[counter] != NULL)
        {
            fprintf(stdout,"%s\n",argv[counter]);
            counter++;
        }
        if (getpid() == pid_shell)
        {
            flag_stp = 1;
            return;
        }
        exit(SUCCESS_EXIT);
    }
    if (sig == SIGTERM)
    {
        flag_exit = 1;
        flag_ending = 1;
        return;
    }
    if (sig = SIGCHLD)
    {
        flag_continue = 1;
        return;
    }
}

int main()
{
    stdin_orig = dup(0);
    stdout_orig = dup(1);
    pid_shell = getpid();
    shell_loop();
    return SUCCESS_EXIT;
}

void shell_loop()
{
    char * line;
    int argc;
    for(;;)
    {
        signal(SIGINT,SIG_IGN);
        signal(SIGCHLD,SIG_IGN);
        signal(SIGTSTP,SIG_IGN);
        signal(SIGTERM,Father_handler_one);
        signal(SIGUSR1,Father_handler_one);
        line = read_commands();
        flag_only = 0;
        flag_std = 0;
        flag_continue = 0;
        flag_stp = 0;
        flag_exit = 0;
        if (empty_line(line) == 1)
        {
            free_input_line(line);
            continue;
        }
        argc = amount_of_deviders(line);
        if (argc == 1)
        {
            flag_only = 1;
        }
        run_commands(line,argc,line);
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

void run_commands(char * line,int argc,char * line_for_cleaning)
{
    pid2 = 0;
    pid1 = 0;
    int fd[2];
    int position = 0;
    int end = position;
    if (check(line,argc) == FAULT_EXIT)
    {   
        fprintf(stderr,"::Error in current command\n");
        if (getpid() == pid_shell)
        {
            return;
        }
        exit(FAULT_EXIT);
    }
    if (is_there_std(line,position) == 1)
    {
        char symbol;
        int ans;
        flag_std = 1;
        ans = skip_to_std(line,&end);
        symbol = line[end - ans];
        line[end - ans] = '\0';         
        argv = get_new_command(line);
        line[end - ans] = symbol;
    }
    else
    {
        argv = get_new_command(line);
    }
    skip_to_zero(line,&position);
    if (pipe(fd) == -1)
    {
        fprintf(stderr,"::Error in pipe()\n");
        exit(FAULT_EXIT);
    }
    signal(SIGINT,Father_handler_one);
    signal(SIGTSTP,Father_handler_one);
    signal(SIGCHLD,Father_handler_one);
    pid2 = fork();
    if (pid2 == 0)
    {
        signal(SIGTSTP,SIG_DFL);
        signal(SIGINT,SIG_DFL);
        dup2(fd[1],1);
        if (argc == 1)
        {
            dup2(stdout_orig,1);
        }
        if (flag_std == 1)
        {
            restd(line);
        }
        close(fd[1]);
        close(fd[0]);
        if (not_execable(argv[0]) == 1)
        {
            char * path_3;
            if (strcmp(argv[0],"pwd") == 0)
            {
                if((path_3 = get_current_directory()) == NULL)
                {
                    free_list_of_commands(argv);
                    free_input_line(line_for_cleaning);
                    exit(FAULT_EXIT);
                }
                fprintf(stdout,"%s\n",path_3);
                free(path_3);
                free_input_line(line_for_cleaning);
                free_list_of_commands(argv);
                exit(0);
            }
            else if (flag_only == 1)
            {
                if (strcmp(argv[0],"exit") == 0)
                {
                    free_input_line(line_for_cleaning);
                    free_list_of_commands(argv);
                    kill(getppid(),SIGTERM);
                    exit(SUCCESS_EXIT);
                }
                kill(getppid(),SIGUSR1);
            }
            free_input_line(line_for_cleaning);
            free_list_of_commands(argv);
            exit(SUCCESS_EXIT);
        }
        execvp(argv[0],argv);
        fprintf(stderr,"::Error in exec()\n");
        free_list_of_commands(argv);
        free_input_line(line_for_cleaning);
        exit(FAULT_EXIT);
    }
    else if (pid2 < 0)
    {
        fprintf(stderr,"::Error in fork() in son_executer\n");
        free_list_of_commands(argv);
        if (getpid() == pid_shell)
        {
            return;
        }
        free(line_for_cleaning);
        exit(SUCCESS_EXIT);
    }
    if (flag_continue == 0)
    {
        pause();
    }
    if (flag_stp == 1)
    {
        free_list_of_commands(argv);
        return;
    }
    waitpid(pid2,NULL,0);
    if (flag_exit == 1 || argc == 1)
    {
        free_list_of_commands(argv);
        if (getpid() == pid_shell)
        {
            return;
        }
        free(line_for_cleaning);
        exit(SUCCESS_EXIT);
    }
    signal(SIGINT,SIG_IGN);
    signal(SIGTSTP,SIG_IGN); 
    signal(SIGCHLD,SIG_IGN);
    close(fd[1]);
    pid1 = fork();
    if (pid1 == 0)
    {
        flag_continue = 0;
        flag_std = 0;
        dup2(fd[0],0);
        close(fd[0]);
        argc--;
        free_list_of_commands(argv);
        if (argc == 0)
        {
            exit(SUCCESS_EXIT);
        }
        line += (position + 1);
        run_commands(line,argc,line_for_cleaning);
        exit(SUCCESS_EXIT);
    }
    else if (pid1 < 0)
    {
        fprintf(stderr,"::Error in fork() in son_continuer\n");
    }
    close(fd[0]);
    waitpid(pid1,NULL,0);
    free_list_of_commands(argv);
    if (getpid() != pid_shell)
    {
        free_input_line(line_for_cleaning);
        exit(SUCCESS_EXIT);
    }
    return;   
}

void go_to_directory(char ** argv)
{
    if (argv[1] != NULL)
    {
        if (argv[2] != NULL)
        {
            fprintf(stderr,"::Error in cd\n");
            return;
        }
    }
    else 
    {
        fprintf(stderr,"::Error in cd\n");
        return;
    }
    int counter = 0;
    char * path;
    path = get_current_directory();
    int len = 0;
    while (path[len] != '\0')
    {
        len++;
    }
    path[len] = '/';
    while(argv[1][counter] != '\0')
    {
        path[len + 1 + counter] = argv[1][counter];
        counter++;
    }
    path[len + 1 + counter] = '\0';
    if (chdir(path) == -1)
    {
        fprintf(stderr,"::Error in going to directory\n");
    }
    free(path);
    return;
}

char * get_current_directory(void)
{
    char * current_path = NULL;
    current_path = (char *)malloc(LEN * sizeof(char));
    if (!current_path)
    {
        fprintf(stderr,"::Error in alloc memory\n");
        return NULL;
    }
    if (getcwd(current_path,LEN - 1) == NULL)
    {
        fprintf(stderr,"::Error in getwd()\n");
        free_input_line(current_path);
        return NULL;
    }
    return current_path;
}

int not_execable(char * command)
{
    if (strcmp(command,"pwd") == 0)
    {
        return 1;
    }
    if (strcmp(command,"exit") == 0)
    {
        return 1;
    }
    if (strcmp(command,"cd") == 0)
    {
        return 1;
    }
    return 0;
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

int amount_of_deviders(char * line)
{
    int position = 0;
    int counter = 0;
    while (line[position] != '\0')
    {
        if (line[position] == DEVIDER)
        {
            counter++;
        }
        position++;
    }
    return (++counter);
}

int check_line_on_stds(char * line)
{
    int position = 0;
    if (is_std_first(line,&position) == 1)
    {
        return FAULT_EXIT;
    }
    while (is_there_std(line,position) != 0)
    {   
        if (is_there_std(line,position) == -1)
        {
            return FAULT_EXIT;
        }
        skip_to_std(line,&position);
        if (check_smt_after_std(line,position) == 1)
        {
                return FAULT_EXIT;
        }
        if (is_there_std(line,position) == -1)
        {
            return FAULT_EXIT;
        }
        if (is_there_std(line,position) == 1)
        {
            continue;
        }
        skip_to_zero(line,&position);
        break;
    }
    return SUCCESS_EXIT;
}

void restd(char * line)
{
    int start = 0;
    int end = start;
    int ans;
    char std[3];
    ans = skip_to_std(line,&end);
    do
    {
        if (ans == 1)
        {
            std[0] = line[end - 1];
            std[1] = '\0';
        }
        else if (ans == 2)
        {
            std[0] = line[end - 2];
            std[1] = line[end - 1];
            std[2] = '\0';
        }
        start = end;
        ans = skip_to_std(line,&end);
        end -= (ans + 1);
        redup(line,start,end,std);
        if (ans == 0)
        {
            break;
        }
        end += (ans + 1);
    }   
    while (1);
}

void redup(char * line,int start,int end,char std[3])
{
    while (line[start] == ' ' || line[start] == '\t')
    {
        start++;
    }
    while (line[end] == ' ' || line[end] == '\t')
    {
        end--;
    }
    char file_name[end - start + 2];
    str_copy(file_name,line,start,end);
    if ((std[1] == '\0' && std[0] == '<') || (std[1] != '\0' && std[0] == '>'))
    {
        fd = open(file_name,O_RDWR);
        if (fd == -1)
        {
            fd = open(file_name, O_CREAT | O_RDWR);
            if (fd == -1)
            {
                perror("::Error in open()\n");
                return;
            }
        }
    }
    else
    {
        fd = open(file_name,O_RDWR | O_TRUNC);
        if (fd == -1)
        {
            fd = open(file_name, O_CREAT | O_RDWR);
            if (fd == -1)
            {
                perror("::Error in open()\n");
                return;
            }
        }
    }
    if (std[1] == '\0')
    {
        if (std[0] == '<')
        {
            dup2(stdin_orig,0);
            dup2(fd,0);
            close(fd);
        }
        else
        {
            dup2(stdout_orig,1);
            dup2(fd,1);
            close(fd);
        }
        return;
    }
    else
    {
        if (std[0] == '2')
        {
            dup2(fd,2);
        }
        else
        {
            lseek(fd,0,SEEK_END);
            dup2(stdout_orig,1);
            dup2(fd,1);
        }
        return;
    } 
}

int check(char * line, int argc)
{
    int position = 0;
    if (is_there_devider_first(line,position) == 1)
    {
        return FAULT_EXIT;
    }
    if (skip_to_devider(line,&position) == 1)
    {
        if (argc != 1)
        {
            return FAULT_EXIT;
        }
    }
    line[position] = '\0';
    if (check_line_on_stds(line) == FAULT_EXIT)
    {
        return FAULT_EXIT;
    } 
    return SUCCESS_EXIT;
}