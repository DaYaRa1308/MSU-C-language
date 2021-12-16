#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct for_play{
	int val;
}game;

game * ball_count;

int read_argv(char *);

void handler(int sig)
{
	if (sig == SIGUSR1)
	{
		shmdt(ball_count);
	}
}

int main(int argc, char ** argv)
{
	if (argc != 3)
	{
		return 0;
	}
	signal(SIGUSR1,handler);
	int i;
	key_t key;
	int shmid;
	int semid;
	size_t size;
	int amount;
	int maxval;
	struct sembuf sops[1] = {0,0,0};
	key = ftok("/etc/passwd",'a');
	size = sizeof(game)/getpagesize() + 1;
	shmid = shmget(key,size,IPC_CREAT|0664);
	ball_count = (game *)shmat(shmid,NULL,0);
	amount = read_argv(argv[1]);
	maxval = read_argv(argv[2]);
	pid_t children[amount];
	semid = semget(key,amount + 1,IPC_CREAT|0664);
	ball_count->val = 0;
	for (i = 0; i<amount; i++)
	{
		children[i] = fork();
		if (!children[i])//son
		{
			int whom;
			sops[0].sem_num = i;
			sops[0].sem_op = -1;
			if (i == 0)
			{
				fprintf(stdout,"%d: %d\n",i,ball_count->val);
				ball_count->val++;
				whom =(ball_count->val * ball_count->val) % amount;
				sops[0].sem_num = whom;
				sops[0].sem_op = 1;
				semop(semid,sops,1);
				sops[0].sem_op = -1;
				sops[0].sem_num = i;
			}
			for(;;)
			{
				semop(semid,sops,1);
				if (ball_count->val >= maxval)
				{
					sops[0].sem_num = amount;
					sops[0].sem_op = 1;
					shmdt(ball_count);
					semop(semid,sops,1);
					exit(0);
				}
				fprintf(stdout,"%d: %d\n",i,ball_count->val);
				ball_count->val++;
				whom = (ball_count->val * ball_count->val) % amount;
				sops[0].sem_num = whom;
				sops[0].sem_op = 1;
				semop(semid,sops,1);
				sops[0].sem_op = -1;
				sops[0].sem_num = i;
			}
		}
	}
	//flg,op,num
	sops[0].sem_op = -1;
	sops[0].sem_num = amount;
	semop(semid,sops,1);
	for (i = 0; i<amount; i++)
	{
		kill(children[i],SIGUSR1);
		kill(children[i],SIGKILL);
		wait(NULL);
	}
	shmdt(ball_count);
	semctl(semid,0,IPC_RMID,0);
	shmctl(shmid,IPC_RMID,NULL);
	return 0;
}

int read_argv(char * str)
{
	int i = 0;
	int rez = 0;
	while (str[i] != '\0')
	{
		rez = rez * 10 + str[i] - '0';
		i++;
	}
	return rez;
}

