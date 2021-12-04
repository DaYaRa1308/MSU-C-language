#include <stdio.h>

int main()
{
	long long a,b;
	int n;
	long long i,j;
	scanf(" %lld %lld %d",&a,&b,&n);
	if (n <= 0)
	{
		return 0;
	}
	for (i = 0;i < n+1;i++)
	{
		printf(" ");
	}
	for (i = a; i < b; i++)
	{
		printf("%*lld",n,i);
		if (i == (b - 1))
		{
			break;  
		}
		printf(" ");
	}
	printf("\n");
	for (i = a; i < b; i++)
	{
		printf("%*lld",n,i);
		printf(" ");
		for (j = a; j < b; j++)
		{
			printf("%*lld",n,(i*j));
			if (j == (b - 1))
			{	
				break;  
			}
			printf(" ");
		}
		printf("\n");
	}
	return 0;
}