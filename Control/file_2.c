#include <stdio.h>
#include <stdlib.h>
#define LEN 1024

char * devider_file(char *,char *,char *);
int down_str(int,char *);

int main(int argc, char ** argv)
{
	if (argc != 4)
	{
		return 0;
	}
	char * token;
	token = devider_file(argv[1],argv[2],argv[3]);
	if (token == NULL)
	{
		return 0;
	}
	fprintf(stdout,"token_file is %s",token);
	return 0;
}

char * devider_file(char * file_name, char * file_ans, char * delim)
{
	int c = 100;
	FILE * fp;
	FILE * fp_ans;
	char * buffer; 
	int bufsize = LEN;
	int len_cur_word = 0;
	fp = fopen(file_name,"r");
	if (!fp)
	{
		return NULL;
	}
	fp_ans = fopen(file_ans,"w");
	if (!fp_ans)
	{
		fclose(fp);
		return NULL;
	}
	buffer = (char *)malloc(bufsize * sizeof(char));
	if (!buffer)
	{
		return NULL;
	}
	for (;;)
	{
		c = getc(fp);
		if (c == EOF)
		{
			buffer[len_cur_word] = '\0';
			break;
		}
		if (down_str(c,delim) == 1)
		{
			buffer[len_cur_word] = '\0';
			fprintf(fp_ans,"%s\n",buffer);
			free(buffer);
			len_cur_word = 0;
			bufsize = LEN;
			buffer = (char *)malloc(bufsize*sizeof(char));
			if (!buffer)
			{
				return NULL;
			}
		}
		else
		{
			buffer[len_cur_word] = (char)c;
			len_cur_word++;
			if (len_cur_word >= bufsize)
			{
				bufsize += LEN;
				buffer = (char *)realloc(buffer,bufsize*sizeof(char));
				if (!buffer)
				{
					return NULL;
				}
			}
		}
	}
	return buffer;
}

int down_str(int c,char * delim)
{
	int i = 0;
	while (delim[i] != '\0')
	{
		if ((char)c == delim[i])
		{
			return 1;
		}
		i++;
	}
	return 0;
}
