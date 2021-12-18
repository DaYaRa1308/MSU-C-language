#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#define SUCCES 0

int flag_end = 0;
int flag_find = 0;
long long last;

void handler (int sig)
{
    if (sig ==  SIGTERM)
    {
        exit(SUCCES);
    }
    if (sig == SIGINT)
    {
        if (flag_end == 3)
        {
            exit(SUCCES);
        }
        if (flag_find == 0)
        {
            fprintf(stdout,"0");
            fflush(stdout);
        }
        else
        {
            fprintf(stdout,"%lld",last);
            fflush(stdout);
        }
        flag_end++;
    }
}

int simple(long long number)
{
    if (number < 0)
    {
        number *= (-1);
    }
    long long i;
    for (i = 2; i < number; i++)
    {
        if ((number % i) == 0)
        {
            return 0;
        }
    }
    return 1;
}

int main()
{
    signal(SIGINT,handler);
    signal(SIGTERM,handler);
    long long i;
    long long low,high;
    if (scanf(" %lld %lld",&low,&high) != 2)
    {
        return 0;
    } 
    fprintf(stdout,"%d",getpid());
    fflush(stdout);
    for (i = low; i < high; i++)
    {
	//sleep(1);
        if (simple(i) == 1)
        {
            flag_find = 1;
            last = i;
            fprintf(stdout,"%lld",i);
            fflush(stdout);
        }
    }
    signal(SIGINT,SIG_IGN);
    signal(SIGTERM,SIG_IGN);
    fprintf(stdout,"-1");
    fflush(stdout);
    return 0;
}
