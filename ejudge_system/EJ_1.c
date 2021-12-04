#include <stdio.h>

int main()
{
	int in;
	int sum = 0;
	while ((in = getchar()) != EOF)
	{
		if (in >= '0' && in <='9')
		{
			sum += (int)in - '0';
		}
		else if (in >= 'a' && in <= 'f')
		{
			sum += (int)in - 'a' + 10;
		}
		else if (in >= 'A' && in <= 'F')
		{
			sum += (int)in - 'A' + 10;
		}
	}
	printf("%d\n",sum);
	return 0;
}