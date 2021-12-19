#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#define AMOUNT 3

int read_argv(char *);

int main(int argc, char ** argv)
{
	if (argc != 2)
	{
		return 0;
	}
	int max_cnt = read_argv(argv[1]);
	if (max_cnt == -1)
	{
		return 0;
	}
	int fd[2];
	key_t key;
	key = ftok("/etc/passwd",'a');
	int semid;
	int flag_end = 0;
	int begin = 1;
	int marker[2];
	marker[0] = 0;
	marker[1] = 1;
	int value;
	semid = semget(key,AMOUNT,IPC_CREAT|0664);
	struct sembuf sops = {0,0,0};
	pipe(fd);
	fprintf(stdout,"father %d\n",getpid());
	if (!fork())
	{
		if (!fork())
		{
			fprintf(stdout,"grandson %d\n",getpid());
			for(;;)
			{
				sops.sem_num = 2;
				sops.sem_op = -1;
				semop(semid,&sops,1);
				if (begin == 0)
				{
					read(fd[0],marker,2*sizeof(int));
				}
				else
				{
					begin = 0;
				}
				if ((marker[1] + marker[0]) > max_cnt)
				{
					flag_end = 1;
				}
				else
				{
					value = marker[1] + marker[0];
					marker[0] = marker[1];
					marker[1] = value;
					fprintf(stdout,"%d %d\n",getpid(),value);
				}
				write(fd[1],marker,2 * sizeof(int));		
				sops.sem_num = 1;
				sops.sem_op = 1;
				semop(semid,&sops,1);
				if (flag_end == 1)
				{
					close(fd[0]);
					close(fd[1]);
					exit(0);
				}
			}
		}
		fprintf(stdout,"son %d\n",getpid());
		for(;;)
		{
			sops.sem_num = 1;
			sops.sem_op = -1;
			semop(semid,&sops,1);
			read(fd[0],marker,2*sizeof(int));
			if ((marker[1] + marker[0]) > max_cnt)
			{
				flag_end = 1;
			}
			else
			{
				value = marker[0] + marker[1];
				marker[0] = marker[1];
				marker[1] = value;
				fprintf(stdout,"%d %d\n",getpid(),value);
			}
			write(fd[1],marker,2*sizeof(int));
			sops.sem_num = 0;
			sops.sem_op = 1;
			semop(semid,&sops,1);
			if (flag_end == 1)
			{
				wait(NULL);
				sops.sem_num = 0;
				sops.sem_op = 1;
				semop(semid,&sops,1);
				close(fd[1]);
				close(fd[0]);
				exit(0);
			}
		}
	}
	sops.sem_num = 2;
	sops.sem_op = 1;
	semop(semid,&sops,1);
	for(;;)
	{
		sops.sem_num = 0;
		sops.sem_op = -1;
		semop(semid,&sops,1);
		read(fd[0],marker,2*sizeof(int));
		if ((marker[1] + marker[0]) > max_cnt)
		{
			flag_end = 1;
		}
		else
		{
			value = marker[0] + marker[1];
			marker[0] = marker[1];
			marker[1] = value;
			fprintf(stdout,"%d %d\n",getpid(),value);
		}
		write(fd[1],marker,2*sizeof(int));
		sops.sem_num = 2;
		sops.sem_op = 1;
		semop(semid,&sops,1);
		if (flag_end == 1)
		{
			wait(NULL);
			break;
		}
	}
	semctl(semid,0,IPC_RMID,0);
	close(fd[0]);
	close(fd[1]);
	return 0;
}

int read_argv(char * num)
{
	int rez = 0;
	int i = 0;
	while (num[i] != '\0')
	{
		if (!((num[i] >= '0') && (num[i] <= '9')))
		{
			return -1;
		}
		rez = rez * 10 + num[i] - '0';
		i++;
	}
	return rez;
}
