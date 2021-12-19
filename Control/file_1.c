#include <stdio.h>
#include <stdlib.h>

char * getline_l(FILE * f)
{
	int c;
	int len = 1024;
	int counter = 1;
	c = getc(f);
	if (c == EOF)
	{
		return NULL;
	}
	int bufszie = len;
	char * line = (char *)malloc(bufszie*sizeof(char));
	if (!line)
	{
		return NULL;
	}
	line[0] = (char)c;
	counter = 1;
	while (!(c == '\n'))
	{
		c = getc(f);
		if (c == EOF)
		{
			break;
		}
		line[counter] = (char)c;
		counter++;
		if (counter >= bufszie)
		{
			bufszie += len;
			line = (char *)realloc(line,bufszie*sizeof(char));
			if (!line)
			{
				return NULL;
			}
		}	
	}
	return line;
}

int main()
{
	return 0;
}
