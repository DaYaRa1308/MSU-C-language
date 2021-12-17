#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

long long read_argv(char *);

int flag_cont = 0;

void handler(int sig)
{
	flag_cont = 1;
	return;
}

int main(int argc, char ** argv)
{
	signal(SIGUSR1,handler);
	struct
	{
		long msgtype;
		long long values[2];
	} message;
	struct 
	{
		long msgtype;
		long long values[2];
	} message_get;
	if (argc != 5)
	{
		return 0;
	}
	long long value3;
	long long value1 = read_argv(argv[2]);
	long long value2 = read_argv(argv[3]);
	long long maxval = read_argv(argv[4]);
	long long amount = read_argv(argv[1]);
	key_t key;
	int msgid;
	int i;
	pid_t children[amount];
	key = ftok("/etc/passwd",'a');
	msgid = msgget(key,IPC_CREAT|0664);
	for (i = 0; i < amount; i++)
	{
		children[i] = fork();
		if (!children[i])
		{
			for (;;)
			{
				msgrcv(msgid,&message_get,sizeof(message_get)-sizeof(long),i+1,0);
				value3 = message_get.values[0]+message_get.values[1];
				fprintf(stdout,"%d %lld\n",i,value3);
				if (value3 > maxval)
				{
					kill(getppid(),SIGUSR1);
					pause();
				}
				message.msgtype = value3%(long long)amount + 1;
				message.values[0] = message_get.values[1];
				message.values[1] = value3;
				msgsnd(msgid,&message,sizeof(message)-sizeof(long),0);
			}		
		}
	}
	message.msgtype = 1;
	message.values[0] = value1;
	message.values[1] = value2;
	msgsnd(msgid,&message,sizeof(message)-sizeof(long),0);
	if (flag_cont == 0)
	{
		pause();
	}
	for (i = 0; i < amount; i++)
	{
		kill(children[i],SIGKILL);
		wait(NULL);
	}
	msgctl(msgid,IPC_RMID,NULL);
	return 0;
}

long long read_argv(char * num)
{
	int i = 0;
	long long rez = 0;
	while (num[i] != '\0')
	{
		rez = rez * 10 + num[i] - '0';
		i++;
	}
	return rez;
}
