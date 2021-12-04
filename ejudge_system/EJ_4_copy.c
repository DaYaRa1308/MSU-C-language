#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

FILE ** get_all_files(int argc, char * argv[]);
int print_from_files(FILE ** fp, int argc);

int main(int argc, char *argv[])
{
	if (argc <= 1)
	{
		return 0;
	}
	int counter = 0;
	FILE ** fp = NULL;
	fp = get_all_files(argc,argv);
	if (fp == NULL)
	{
		return 1;
	}
	if (print_from_files(fp,argc) == 1)
	{
		while (counter != (argc - 1))
		{
			if (*(fp + counter) != NULL)
			{
				fclose(*(fp + counter));
				counter++;
			}
		}
		free(fp);
		return 1;
	}
	return 0;
}

FILE ** get_all_files(int argc, char * argv[])
{
	int counter = 0;
	FILE ** fp = (FILE **)malloc((argc - 1) * sizeof(FILE *));
	if (fp == NULL)
	{
		return NULL;
	}
	while (counter != (argc - 1))
	{
		*(fp + counter) = fopen(argv[counter + 1],"r");
		if (*(fp + counter) == NULL)
		{
			counter = 0;
			while (*(fp + counter) != NULL)
			{
				fclose(*(fp + counter));
				counter++;
			}
			free(fp);
			return NULL;
		}
		counter++;
	}
	return fp;
}

int print_from_files(FILE ** fp, int argc)
{
	int success_read = -1;
	long long min = LLONG_MAX;                      
	int counter = 0;
	int files_write = 0;
	long long number = 0;
	long long ** numbers = (long long **)malloc((argc - 1) * sizeof(long long *));
	if (numbers == NULL)
	{
		return 1;
	}
	while (counter != (argc - 1))
	{
		if (fscanf(*(fp + counter)," %lld",&number) == 1)
		{
			*(numbers + counter) = (long long *)malloc(sizeof(long long));
			if (*(numbers + counter) == NULL)
			{
				while (counter != -1)
				{
					if (*(numbers + counter) != NULL)
					{
						free(*(numbers + counter));
					}
					counter--;
				}
				free(numbers);
				return 1;
			}
			**(numbers + counter) = number;
		}
		else
		{
			fclose(*(fp + counter));
			*(fp + counter) = NULL;
			*(numbers + counter) = NULL;
		}
		counter++;
	}
	while(success_read != 0)
	{
		success_read = 0;
		min = LLONG_MAX;
		counter = 0;
		while (counter != (argc - 1))
		{
			if (*(numbers + counter) != NULL)
			{
				success_read++;
				if (**(numbers + counter) < min)
				{
					min = **(numbers + counter);
					files_write = counter;
				}
			}
			counter++;
		}
		if (success_read != 0)
		{	
			printf("%lld\n",min);
			if (fscanf(*(fp + files_write)," %lld",&number) == 1)
			{
				**(numbers + files_write) = number;
			}
			else
			{
				fclose(*(fp + files_write));
				*(fp + files_write) = NULL;
				free(*(numbers + files_write));
				*(numbers + files_write) = NULL;
			}
		}
	}
	free(fp);
	free(numbers);
	return 0;
}