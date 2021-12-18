#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>

int read_argv(char *);

int main(int argc,char ** argv)
{
	if (argc != 2)
	{
		return 0;
	}
	int cnt = 0;
	int max_cnt = read_argv(argv[1]);
	int fd_1[2];
	int fd_2[2];
	int ans;
	pipe(fd_1);
	pipe(fd_2);
	if (!fork())
	{
		close(fd_1[0]);
		close(fd_2[1]);
		for(;;)
		{	
			ans = read(fd_2[0],&cnt,sizeof(int));
			cnt++;
			if (cnt >= max_cnt || ans == 0)
			{
				close(fd_1[1]);
				close(fd_2[0]);
				break;
			}
			fprintf(stdout,"1 %d\n",cnt);
			write(fd_1[1],&cnt,sizeof(int));
		}
		exit(0);
	}
	if (!fork())
	{
		close(fd_1[1]);
		close(fd_2[0]);
		for(;;)
		{
			ans = read(fd_1[0],&cnt,sizeof(int));
			cnt++;
			if (cnt >= max_cnt || ans == 0)
			{
				close(fd_1[0]);
				close(fd_2[1]);
				break;
			}
			fprintf(stdout,"2 %d\n",cnt);
			write(fd_2[1],&cnt,sizeof(int));
		}
		exit(0);
	}
	write(fd_2[1],&cnt,sizeof(int));
	close(fd_1[0]);
	close(fd_1[1]);
	close(fd_2[0]);
	close(fd_2[1]);
	while (wait(NULL) != -1);
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
