#include <stdio.h>
#include <signal.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#define AMOUNT 1

int main()
{
    int N;
    int i;
    pid_t pid,pid_father;
    pid_father = getpid();
    key_t key;
    int sem_id;
    struct sembuf action[2] = {{0,-1,0},{0,1,0}};
    if (fscanf(stdin,"%d",&N) != 1)
    {
        return 0;
    }
    if (N <= 0)
    {
        return 0;
    }
    key = ftok("/etc/passwd",'h');
    sem_id = semget(key,AMOUNT,IPC_CREAT | 0664);
    for (i = 0; i < N; i++)
    {
        if (i == (N - 1))
        {
            fprintf(stdout,"%d\n",i+1);
            exit(0);
        }
        pid = fork();
        if (pid > 0)
        {
            fprintf(stdout,"%d ",(i+1));
            fflush(stdout);
            semop(sem_id,action + 1,1);
            waitpid(pid,NULL,0);
            if (getpid() == pid_father)
            {
                semctl(sem_id,0,IPC_RMID,0);
            }
            exit(0);
        }
        else if (pid == 0)
        {
            semop(sem_id,action,1);
            continue;
        }
        else
        {
            exit(0);
        }
    }
    return 0;
}