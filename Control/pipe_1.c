#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>

int flag_out = 0;

int read_number();

int main()
{
	int fd[2];
	int num;
	pipe(fd);
	if (!fork())
	{
		if (!fork())
		{
			int ans;
			long long sum = 0;
			close(fd[1]);
			for(;;)
			{
				ans = read(fd[0],&num,sizeof(int));
				if (ans == 0)
				{
					break;
				}
				sum += (long long)num;
			}
			if (sum != 0)
			{
				fprintf(stdout,"%lld\n",sum);
			}
			exit(0);
		}
		close(fd[1]);
		close(fd[0]);
		wait(NULL);
		exit(0);	
	}
	close(fd[0]);
	while ((num = read_number()) != -1)
	{
		write(fd[1],&num,sizeof(int));
		if (flag_out == 1)
		{
			break;
		}
	}
	close(fd[1]);
	wait(NULL);
	return 0;
}

int read_number()
{
	int ans;
	int rez = 0;
	char symbol = ' ';
	while(symbol == ' '||symbol == '\t'||symbol == '\n')
	{
		ans = read(0,&symbol,sizeof(char));
		if (ans == 0)
		{
			return -1;
		}
	}
	while ((symbol >= '0')&&(symbol <= '9'))
	{
		rez = rez * 10 + symbol - '0';
		ans = read(0,&symbol,sizeof(char));
		if (ans == 0)
		{
			flag_out = 1;
			return rez;
		}
	}
	return rez;
}


