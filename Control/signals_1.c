#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>

int cnt = 1;
int max_cnt;
int fd[2];
int i;
int stdout_father;
pid_t children[2];

int read_argv(char *);
void handler_1(int);
void handler_2(int);
void handler_3(int);
void handler_4(int);

void handler_1(int sig)
{
	if (cnt >= max_cnt)
	{
		kill(getppid(),SIGUSR1);
		exit(0);
	}
	fprintf(stdout,"1 %d\n",cnt);
	write(fd[1],&cnt,sizeof(int));
	kill(getppid(),SIGUSR1);
	signal(SIGUSR1,handler_2);
}

void handler_2(int sig)
{
	int ans;
	if (cnt >= (max_cnt - 1))
	{
		close(fd[1]);
	}
	ans = read(fd[0],&cnt,sizeof(int));
	cnt++;
	if (ans == 0 || cnt >= max_cnt)
	{
		close(fd[0]);
		close(fd[1]);
		kill(getppid(),SIGUSR1);
		exit(0);
	}
	fprintf(stdout,"%d %d\n",i,cnt);
	write(fd[1],&cnt,sizeof(int));
	kill(getppid(),SIGUSR1);
}

void handler_3(int sig)
{
	kill(children[1],SIGUSR1);
	signal(SIGUSR1,handler_4);
}

void handler_4(int sig)
{
	kill(children[0],SIGUSR1);
	signal(SIGUSR1,handler_3);
}

int main(int argc,char ** argv)
{
	if (argc != 2)
	{
		return 0;
	}
	stdout_father = dup(1);
	signal(SIGUSR1,handler_1);
	max_cnt = read_argv(argv[1]);
	pipe(fd);
	if ((children[0] = fork()) == 0)
	{
		i = 1;
		for(;;);
	}
	signal(SIGUSR1,handler_2);
	if ((children[1] = fork()) == 0)
	{
		dup2(stdout_father,1);
		i = 2;
		for(;;);
	}
	close(fd[1]);
	close(fd[0]);
	signal(SIGUSR1,handler_3);
	kill(children[0],SIGUSR1);
	waitpid(children[0],NULL,0);
	waitpid(children[1],NULL,0);
	fprintf(stdout,"Done\n");
	return 0;
}

int read_argv(char * num)
{
	int rez = 0;
	int i = 0;
	while (num[i] != '\0')
	{
		rez = rez * 10 + num[i] - '0';
		i++;
	}
	return rez;
}
