#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>

#define AMOUNT 3
#define LEN 256

typedef struct s_m{
    char txt[256];
}share_memory;

share_memory * text;

void handler(int sig)
{
    if (sig == SIGUSR1)
    {
        shmdt(text);
    }
}

int main()
{
    srand((unsigned int)time(NULL));
    int i;
    key_t key_mem;
    key_t key_sem;
    int idn;
    size_t size;
    pid_t pid1,pid2;
    int num;
    struct sembuf action[6] = {{0,1,0},{0,-1,0},{1,1,0},{1,-1,0},{2,1,0},{2,-1,0}};
    int sem_id;
    key_mem = ftok("/etc/passwd",'a');
    size = sizeof(share_memory)/getpagesize() + 1;
    idn = shmget(key_mem,size,IPC_CREAT|0664);
    text = (share_memory *)shmat(idn,NULL,0);
    key_sem = ftok("/etc/passwd",'b');
    sem_id = semget(key_sem,AMOUNT,IPC_CREAT|0664);
    for (i = 0; i < LEN; i++)
    {
        text->txt[i] = 'a';
    }
    pid1 = fork();
    if (pid1 == 0)
    {
        for(;;)
        {
            num = (rand() % 255);
            while (text->txt[num] == ' ')
            {
                num = (rand() % 255);
            }
            text->txt[num] = ' ';
            semop(sem_id,action,1);
            semop(sem_id,action+3,1);
        }
    }
    pid2 = fork();
    if (pid2 == 0)
    {
        for(;;)
        {
            semop(sem_id,action + 5,1);
            num = (rand() % 255);
            while (text->txt[num] == ' ')
            {
                num = (rand() % 255);
            }
            text->txt[num] = ' ';
            semop(sem_id,action,1);
        }
    }
    int flag_exit = 0;
    int counter;
    for(;;)
    {
        semop(sem_id,action+1,1);
        for (i = 0; i < LEN; i++)
        {
            if (text->txt[i] == 'a')
            {
                continue;
            }
            else
            {
                if (i >= (LEN - 3))
                {
                    flag_exit = 1;
                    break;
                }
                counter = 3;
                while (counter != 0)
                {
                    if (text->txt[i+counter] == ' ')
                    {
                        flag_exit = 1;
                    }
                    counter--;
                }
            }
        }
        if (flag_exit == 1)
        {
            fprintf(stdout,"%d\n",pid1);
            break;
        }
        semop(sem_id,action+4,1);
        semop(sem_id,action+1,1);
        for (i = 0; i < LEN; i++)
        {
            if (text->txt[i] == 'a')
            {
                continue;
            }
            else
            {
                if (i >= (LEN - 3))
                {
                    flag_exit = 1;
                    break;
                }
                counter = 3;
                while (counter != 0)
                {
                    if (text->txt[i+counter] == ' ')
                    {
                        flag_exit = 1;
                    }
                    counter--;
                }
            }
        }
        if (flag_exit == 1)
        {
            fprintf(stdout,"%d\n",pid2);
            break;
        }
        semop(sem_id,action+2,1);
    }
    kill(pid1,SIGUSR1);
    kill(pid2,SIGUSR1);
    kill(pid1,SIGKILL);
    kill(pid2,SIGKILL);
    waitpid(pid1,NULL,0);
    waitpid(pid2,NULL,0);
    semctl(sem_id,0,IPC_RMID,0);
    shmdt(text);
    shmctl(idn,IPC_RMID,NULL);
    return 0;
}
