#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#define LEN_CONFIG 100
#define POS_OF_I 7
#define LEN_MESSAGE 1025

int count_from_terminal(char *);
int read_number_of_son(int);
int read_message(int, char *);

int flag_going_on = 0;
int flag_pause = 0;
int flag_write = 0;
int flag_read = 0;
int fd[2];

void SigSonHandler(int sig)
{                                      
    if (sig == SIGUSR1)
    {
        flag_write = 1;
        return;
    }
    else if (sig == SIGUSR2)
    {
        flag_read = 1;
        signal(SIGUSR2,SigSonHandler);
        return;
    }
    else if (sig == SIGTERM)
    {
        close(fd[1]);
        close(fd[0]);
        signal(SIGTERM,SigSonHandler);
        exit(0);
    }
}

void SigFatherHandler(int sig)
{  
    if (sig == SIGUSR1)
    {
        flag_pause = 1;
        signal(SIGUSR1,SigFatherHandler);
        return;
    }
    else if (sig == SIGUSR2)
    {
        flag_pause = 1;
        flag_going_on = 1;
        signal(SIGUSR2,SigFatherHandler);
        return;                                      
    }
    else if (sig == SIGCHLD)
    {
        signal(SIGCHLD,SigFatherHandler);
        return;
    }
}

int main(int argc, char ** argv)
{
    pid_t pid;
    int status;
    int i;
    int counter = 0;
    int number;
    int amount_of_processes;
    pid_t * children = NULL;
    int son_err = 0;
    if (argc != 2)
    {
        printf("not enough arguements in terminal or get more arguements ending\n");
        return -1;
    }
    amount_of_processes = count_from_terminal(argv[1]);
    if (amount_of_processes == 0)
    {
        printf("get zero amount of processes ending\n");
        return -1;
    }
    if (amount_of_processes == -1)
    {
        printf("err in amount of processes ending\n");
        return -1;
    }
    if (amount_of_processes == 1)
    {
        printf("get N = 1 ending\n");
        return -1;
    }
    children = (pid_t *)malloc(amount_of_processes * sizeof(pid_t));
    if (children == NULL)
    {
        printf("mem err ending\n");
        return -1;
    }
    if (pipe(fd) == -1)
    {
        printf("err in creating pipe ending\n");
        perror("Err code errno:");
        free(children);
        return -1;
    }
    signal(SIGUSR1,SigSonHandler);
    signal(SIGUSR2,SigSonHandler);
    signal(SIGTERM,SigSonHandler);
    while (counter != (amount_of_processes - 1))
    {
        if ((pid = fork()) > 0)//father
        { 
            counter++;
            *(children + counter) = pid;
            continue;
        }
        else if (pid == 0)//one of sons
        {
            char buf[LEN_MESSAGE];
            char buf_copy[LEN_MESSAGE];
            char config_name[LEN_CONFIG]="config_";
            int fd_son;
            int rez_copy;  
            int copy_copy; 
            int flag_error = 0; 
            int rez = 0;
            int copy;  
            int from_who;
            int len;
            int len_copy;                                  
            counter++;
            free(children);
            if (counter < 10)
            {
                config_name[POS_OF_I] = '0' + counter;
            }
            else if (counter >= 10)
            {
                config_name[POS_OF_I] = '1';
                config_name[POS_OF_I + 1] = '0' + (counter % 10);
            }
            fd_son = open(config_name, O_RDONLY);
            if (fd_son == -1)
            {
                flag_error = 1;   
            }
            if (flag_error != 1)
            {
                number = read_number_of_son(fd_son);
                if (number == 0 || number == -1 || number >= amount_of_processes)
                {
                    flag_error = 1;
                }
            }
            if (flag_error != 1)
            {
                len = read_message(fd_son,buf);
                if (len == -1 || len == 0)
                {
                    flag_error = 1;
                }
            }
            close(fd_son);
            while(1)
            {
                while((!(flag_write == 0 || flag_read == 0)) == 1)pause();
                if (flag_write == 1)
                {
                    if (flag_error == 1)
                    {
                        printf("SON WITH PID %d: err in config_i\n",getpid());
                        flag_write = 0;
                        kill(getppid(),SIGUSR2);
                        continue;
                    }
                    while (write(fd[1],&number,sizeof(int)) == -1);
                    while (write(fd[1],&counter,sizeof(int)) == -1);
                    while (write(fd[1],&len,sizeof(int)) == -1);                                                     
                    while (rez != len)
                    {
                        copy = rez;
                        rez += write(fd[1],(buf + rez),sizeof(char) * (len - rez));
                        if (rez == (copy - 1))
                        {
                            rez++;
                        }
                    }
                    flag_write = 0;
                    kill(getppid(),SIGUSR1);
                    continue;
                }
                if (flag_read == 1)
                {
                    rez_copy = 0;
                    while (read(fd[0],&from_who,sizeof(int)) == -1);
                    while (read(fd[0],&len_copy,sizeof(int)) == -1);
                    copy_copy = rez_copy;
                    while (rez_copy != len_copy)
                    {
                        rez_copy += read(fd[0],buf_copy + rez_copy,(len_copy - rez_copy) * sizeof(char));
                        if (rez_copy == (copy_copy - 1))
                        {
                            rez_copy++;
                        }
                        copy_copy = rez_copy;
                    }
                    buf_copy[len_copy] = '\0';
                    printf("SON №: %d with pid: %d\n",counter,getpid());
                    printf("message get from %d\n",from_who);
                    printf("%s\n",buf_copy); 
                    flag_read = 0;                                   
                    kill(getppid(),SIGUSR1);
                }
            }
        }
        else
        {
            counter++;
            printf("err in creation son: %d\n",counter);
            son_err++;
            continue;
        }
    }
    signal(SIGUSR2,SigFatherHandler);
    signal(SIGUSR1,SigFatherHandler);
    signal(SIGCHLD,SigFatherHandler);
    for (i = 0; i < (amount_of_processes - 1); i++)
    {
        flag_going_on = 0;
        flag_pause = 0;
        if (kill(*(children + i + 1),SIGUSR1) == -1)
        {
            continue;
        }
        while(flag_pause == 0)pause();
        flag_pause = 0;
        if (flag_going_on == 1)
        {
            continue;
        }
        while (read(fd[0],&number,sizeof(int)) == -1);
        if (kill(*(children + number),SIGUSR2) == -1)
        {
            continue;
        }
        while(flag_pause == 0)pause();
    }
    for (i = 0; i < (amount_of_processes - 1); i++)
    {
        if (*(children + i + 1) == -1)
        {
            continue;
        }
        kill(*(children + i + 1),SIGTERM);
    }
    for (i = 0; i < (amount_of_processes - 1 - son_err); i++)
    {
        wait(&status);
    }
    free(children);
    close(fd[0]);
    close(fd[1]);
    return 0;
}

int count_from_terminal(char * str)
{
    int number = 0;
    int counter = 0;
    while (*(str + counter) != '\0')
    {
        if (*(str + counter) >= '0' && *(str + counter) <= '9')
        {
            number = number * 10 + *(str + counter) - '0';
        }
        else
        {
            return -1;
        }
        counter++;
    }
    return number;
}

int read_number_of_son(int fd_son)
{
    char symbol;
    int rez;
    int number = -1;
    int flag_reading = 0;
    while (1)
    {
        rez = read(fd_son,&symbol,sizeof(char));
        if (rez == -1)
        {
            continue;
        }
        else if (rez == 0)
        {
            return -1;
        }
        else
        {
            if (symbol == ' ' || symbol == '\t')
            {   
                if (number != -1)
                {
                    flag_reading = 1;
                }
                continue;
            }
            else if (symbol >= '0' && symbol <= '9')
            {
                if (flag_reading == 1)
                {
                    return -1;
                }
                if (number == -1)
                {
                    number = 0;
                }
                number = number * 10 + symbol - '0';
            }
            else if (symbol == '\n')
            {
                return number;
            }
            else
            {
                return -1;
            }
        }
    }
}

int read_message(int fd_son, char * buf)
{
    int rez;
    char symbol;
    int counter = 0;
    while ((rez = read(fd_son,&symbol,sizeof(char))) != 0)
    {
        if (rez == -1)
        {
            continue;
        }
        else 
        {
            if (counter > LEN_MESSAGE - 1)
            {
                return -1;
            }
            *(buf + counter) = symbol;
            counter++;
        }
    }
    counter--;
    *(buf + counter) = '\0';
    return counter;
}