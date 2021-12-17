#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct s_m{
	int flag_end;
} share_memory;

share_memory * flag;
int flag_exit = 0;

int read_argv(char *);
int read_number();

int main (int argc, char ** argv)
{
	if (argc != 2)
	{
		return 0;
	}
	int number;
	key_t key = ftok("/etc/passwd",'a');
	int amount = read_argv(argv[1]);
	struct sembuf sops = {0,0,0};
	int semid = semget(key,amount + 1,IPC_CREAT|0664);
	size_t size = sizeof(share_memory)/getpagesize() + 1;
	int shmid = shmget(key,size,IPC_CREAT|0664);
	flag = (share_memory *)shmat(shmid,NULL,0);
	flag->flag_end = 0;
	pid_t children[amount];
	int i,j;
	for (i = 0; i < amount; i++)
	{
		children[i] = fork();
		if (!children[i])
		{
			for(;;)
			{
				sops.sem_num = i;
				sops.sem_op = -1;
				semop(semid,&sops,1);
				if (flag->flag_end == 1)
				{
					shmdt(flag);
					exit(0);
				}
				number = read_number();
				if (flag_exit == 1)
				{
					flag->flag_end = 1;
					for (j = 0; j < amount; j++)
					{
						sops.sem_num = j;
						sops.sem_op = 1;
						semop(semid,&sops,1);
					}
					shmdt(flag);
					sops.sem_num = amount;
					sops.sem_op = 1;
					semop(semid,&sops,1);
					exit(0);
				}
				fprintf(stdout,"%d %d\n",i,number);
				fflush(stdout);
				if (flag->flag_end == 1)
				{
					for (j = 0; j < amount; j++)
					{
						sops.sem_num = j;
						sops.sem_op = 1;
						semop(semid,&sops,1);
					}
					shmdt(flag);
					sops.sem_num = amount;
					sops.sem_op = 1;
					semop(semid,&sops,1);
					exit(0);
				}
				sops.sem_num = number%amount;
				sops.sem_op = 1;
				semop(semid,&sops,1);
			}
		}
	}
	sops.sem_num = 0;
	sops.sem_op = 1;
	semop(semid,&sops,1);
	sops.sem_num = amount;
	sops.sem_op = -1;
	semop(semid,&sops,1);
	for (i = 0; i < amount; i++)
	{
		waitpid(-1,NULL,WNOHANG);
	}
	shmdt(flag);
	shmctl(shmid,IPC_RMID,NULL);
	semctl(semid,0,IPC_RMID,0);
	return 0;
}

int read_argv(char * num)
{
	int i = 0;
	int rez = 0;
	while (num[i] != '\0')
	{
		rez = rez * 10 + num[i] - '0';
		i++;
	}
	return rez;
}

int read_number()
{
	char symbol = ' ';
	int rez = 0;
	int ans;
	while (symbol == ' ' || symbol == '\t' || symbol == '\n')
	{
		ans = read(0,&symbol,sizeof(char));
		if (ans == 0)
		{
			flag_exit = 1;
			return -1;
		}
	}
	while ((symbol >= '0') && (symbol <= '9'))
	{
		rez = rez * 10 + symbol - '0';
		ans = read(0,&symbol,sizeof(char));
		if (ans == 0)
		{
			flag->flag_end = 1;
			return rez;
		}
	}
	return rez;
}

