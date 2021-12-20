#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_LEN 1000

int read_argv(char *);

typedef struct share_memory{
	int counter;
	char txt[MAX_LEN];
}shm_mem;

int main(int argc, char ** argv)
{
	if (argc != 3)
	{
		return 0;
	}
	int n = read_argv(argv[1]);
	int m = read_argv(argv[2]);
	if (n == -1 || m == -1)
	{
		return 0;
	}
	shm_mem * str;
	size_t size = sizeof(shm_mem)/getpagesize() + 1;
	key_t key = ftok("/etc/passwd",'a');
	int semid = semget(key,n,IPC_CREAT|0666);
	int shmid = shmget(key,size,IPC_CREAT|0666);
	str = (shm_mem *)shmat(shmid,NULL,0);
	int i;
	char symbol = 'a';
	struct sembuf sops = {0,0,0};
	str->counter = 0;
	for (i = 0;i < (n-1); i++)
	{
		if (!fork())
		{
			for(;;)
			{
				sops.sem_num = i + 1;
				sops.sem_op = -1;
				semop(semid,&sops,1);
				if (str->counter == m)
				{
					sops.sem_num = 0;
					sops.sem_op = 1;
					semop(semid,&sops,1);
					exit(0);
				}
				str->txt[str->counter] = symbol + i + 1;
				str->counter++;
				if (i == (n-2))
				{
					sops.sem_num = 0;
					sops.sem_op = 1;
				}
				else
				{
					sops.sem_num = i + 2;
					sops.sem_op = 1;
				}
				semop(semid,&sops,1);
			}
		}
	}
	str->txt[str->counter] = symbol;
	str->counter++;
	sops.sem_num = 1;
	sops.sem_op = 1;
	semop(semid,&sops,1);
	int flag_repeat = 0;
	for(;;)
	{
		sops.sem_num = 0;
		sops.sem_op = -1;
		semop(semid,&sops,1);
		if (str->counter == m)
		{
			break;
		}
		str->txt[str->counter] = symbol;
		str->counter++;
		sops.sem_num = 1;
		sops.sem_op = 1;
		semop(semid,&sops,1);
	}
	sops.sem_op = 1;
	for(i = 0;i < (n-1);i++)
	{
		sops.sem_num = i+1;
		semop(semid,&sops,1);
		wait(NULL);
	}
	str->txt[str->counter] = '\0';
	fprintf(stdout,"%s\n",str->txt);
	semctl(semid,0,IPC_RMID,0);
	shmctl(shmid,IPC_RMID,NULL);
	return 0;
}

int read_argv(char * num)
{
	int rez = 0;
	int i = 0;
	while(num[i] != '\0')
	{
		if ((num[i]>='0')&&(num[i]<='9'))
		{
			rez = rez * 10 + num[i] - '0';
			i++;
		}
		else
		{
			return -1;
		}
	}
	return rez;
}
