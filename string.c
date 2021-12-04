#include <stdio.h>
#include <stdlib.h>
#define LEN 10

char * input_string();
void strcopy(char *, char *);
int input_devide_action(char *,int *,int *);
int check_input_after(void);
int clean_buffer(void);
void print_string(char *);
char ** devider_string(char *, char, char ***);
int input_string_from_devider(int, char **, char *, int *,int);
int skip_devider_begin(char *, char, int *);
int skip_devider_middle(char *, char, int *);
char ** delete_strings(char ***, int,int);
int str_len(char *);
void sort_by_increase(char **, int);
void sort_by_decrease(char **, int);
int cmp_string(char * , char *);
int read_number(char *);

int main(int argc, char * argv[])
{
	char *txt;
	int flag_EOF = 0;
	int i_d_a_result;
	char ***txt_devided;                                        
	char devider;
	int run = 1,i;
	int flag = 0;
	int action,counts;
	if (argc <= 1)
	{
		printf("Not enough data from termnail, continue with keyboard\n");
	}
	else if (argc >= 3 && argc <= 4)/*обрабатываем на входные данные с терминала*/
	{
		switch(argc)
		{
			case 3:/*если действие 1 или 2*/
					if ((*(argv[2]) - '0') != 2 && (*(argv[2]) - '0') != 1)/*проверяем на действие*/
					{
						printf("Err input in action, continue with keyboard\n");
						break;
					}
					if (*((argv[2] + 1)) != '\0')/*проверяем чтоб 1 арг содержал только 1 символ*/
					{
						printf("Err input in action, continue with keyboard\n");
						break;
					}
					if (*((argv[1] + 1)) != '\0')/*проверяем чтоб 1 арг содержал только 1 символ*/
					{
						printf("Err input in devider, continue with keyboard\n");
						break;
					}
					devider = *(argv[1]);
					action = *(argv[2]) - '0';
					flag = 1;/*устанавливаем флаг присваиваем значения*/
					break;
			case 4:/*если действие 3*/
					if ((*(argv[2]) - '0') != 3)/*проверяем на действие*/
					{
						printf("Err input in action, continue with keyboard\n");
						break;
					}
					counts = read_number(argv[3]);/*считыванем N*/
					if (counts == -1)/*проверка что введено число*/
					{
						printf("Err input in N, continue with keyboard\n");
						break;
					}
					if (*((argv[2] + 1)) != '\0')/*проверяем чтоб 2 арг содержал только 1 символ*/
					{
						printf("Err input in devider, continue with keyboard\n");
						break;
					}
					if ((*(argv[1] + 1)) != '\0')/*1 арг содержит только 1 символ*/
					{
						printf("Err input in devider, continue with keyboard\n");
						break;
					}
					devider = *(argv[1]);
					action = *(argv[2]) - '0';
					flag = 1;/*присваиваем значения устанавливаем флаги*/
					break;
			default:
					break;
		}
	}
	else
	{
		printf("Err data form termnail, continue with keyboard\n");
	}
	printf("Enter new string\n");
	txt = input_string();/*вводим строчку*/
	if (txt == NULL)
	{
		printf("See EOF or memory error, ending\n");
		return 1;
	}/*если ввели EOF*/
	txt_devided = (char ***)malloc(sizeof(char **));/*Ввыделяем память под тройной указатель для массива разд строк*/
	if (txt_devided == NULL)
	{
		printf("memory error, ending\n");
		free(txt);
		return 1;
	}
	while (run)
	{
		printf("Input string:\n");
		print_string(txt);/*Печатаем поступившую строку*/
		printf("\n");
		if (!flag)/*проверяем на наличие параметров с терминала*/
		{
			i_d_a_result = input_devide_action(&devider,&action,&counts);
			if (i_d_a_result == 1)/*считываем праметры разд действия и N*/
			{
				printf("See EOF, ending\n");
				free(txt_devided);
				free(txt);
				return 1;
			}/*если встретили EOF*/
			else if (i_d_a_result == 2)
			{
				flag_EOF = 1;
			}
			printf("\n");
		}
		flag = 0;/*меняем так чтоб дальше вводились параметры с клавиатуры*/
		if (devider == 'q')
		{
			printf("See q, ending\n");
			free(txt_devided);
			free(txt);
			return 0;
		}/*конец работы программы*/
		*txt_devided = devider_string(txt,devider,txt_devided);/*получаем разд строку*/
		if (*txt_devided == NULL)
		{
			continue;
		}/*обрабатывается случай если разд не входит в строку или вся строка состоит из разделителя*/
		i = 0;
		printf("After deviding:\n");
		while (*(*txt_devided + i) != NULL)
		{
			print_string(*(*txt_devided + i));
			i++;
		}/*печатаем получ разд строку*/
		printf("\n");
		*txt_devided = realloc(*txt_devided,sizeof(char *) * (i+1));/*освобождаем лишнюю память*/
		if (*txt_devided == NULL)
		{
			printf("memory error, ending\n");
			free(txt_devided);
			free(txt);
			return 1;
		}
		switch(action)/*обрабатываем действие*/
		{
			case 1: 
					sort_by_increase(*txt_devided,i);
					break;
			case 2: 
					sort_by_decrease(*txt_devided,i);
					break;
			case 3:
					*txt_devided = delete_strings(txt_devided,counts,(i+1));
					if (*txt_devided == NULL)
					{
						free(*txt_devided);
						free(txt);
						free(txt_devided);
						printf("memory error, ending\n");
						return 1;
					}
					break;
			default:
					break;
		}
		i = 0;
		printf("After action:\n");
		while (*(*txt_devided + i) != NULL)
		{
			print_string(*(*txt_devided + i));
			i++;
		}/*печатаем получившуюся строку после действия*/
		i = 0;
		printf("\n");
		while (*(*txt_devided + i) != NULL)
		{
			free(*(*txt_devided + i));
			i++;
		}
		free(*txt_devided);/*очищаем разд строку чтобы не засорять память*/
		if (flag_EOF == 1)
		{
			printf("See EOF, ending\n");
			free(txt_devided);
			free(txt);
			return 1;
		}
	}
	free(txt);
	return 0;
}

char * input_string()/*ввод строки*/
{
	int in;/*прочитать символ*/
	int counter = 0;/*двигаться по массиву*/
	int coef = 2;/*для перевыделения памяти*/
	char * txt = (char *)malloc(LEN * sizeof(char));/*массив символов*/
	if (txt == NULL)
	{
		return NULL;
	}
	char * txt_copy;/*для копирования*/
	while ((in = getchar()) != '\n')
	{
		if (in == EOF)
		{
			free(txt);
			return NULL;
		}/*если встретили EOF*/
		*(txt + counter) = (char)in;/*присвоили очередной символ массиву*/
		counter++;/*увеличили счетчик*/
		if (counter == (((coef -1) * LEN) - 1))/*проверяем хватает ли нам памяти*/
		{
			*(txt + counter) = '\0';/*придаем признак конца строки для грамотной рабоыт функции копирования*/
			txt_copy = (char *) malloc((coef -1) * LEN * sizeof(char));/*выделяем память для копии*/
			if (txt_copy == NULL)
			{
				return NULL;
			}
			strcopy(txt_copy,txt);/*копируем содержимое*/
			free(txt);/*освобождаем память*/
			txt = (char *) malloc (coef * LEN * sizeof(char));/*выделяем больше памяти*/
			if (txt == NULL)
			{
				free(txt_copy);
				return NULL;
			}
			strcopy(txt,txt_copy);/*копируем обратно*/
			free(txt_copy);/*освобождаем выделенную память для копии*/
			coef++;/*увеличиваем коэффицент выделения памяти*/
		}
	}
	if (counter == 0)
	{
		printf("Err, empty string, try again\n");
		return input_string();
	}
	*(txt + counter) = '\0';/*придаем полученной строке признак конца*/
	return txt;/*возвращаем указатель на строку*/
}
void strcopy(char * txt,char * txt_copy)/*для копирования строки*/
{
	while (*txt = *txt_copy)
	{
		txt++;
		txt_copy++;
	}
}

int input_devide_action(char * devider,int * action, int * counts)/*ввдоим данные для работы со строкой*/
{
	int turn_out;
	int in;
	printf("Enter devider, mode and in case mode=3 enter N\n");
	printf("Use devider q for ending program\n");
	while ((scanf("%c %u",devider,action)) != 2)
	{
		printf("Err, try again\n");
		if(clean_buffer() == 1)
		{
			return 1;
		}/*считываем, исправляем ошибки и проверяем на EOF*/
	}
	if (*action < 1 || *action > 3)
	{
		printf("Err,try again\n");
		return input_devide_action(devider,action,counts);
	}
	if (*action == 3)
	{
		while ((scanf(" %u", counts) != 1) || (*counts < 0))
		{
			printf("Err, try again input counts\n");
			if(clean_buffer() == 1)
			{
				return 1;
			}
		}
	}
	turn_out = check_input_after();
	if (turn_out == 2)
	{
		return 2;
	}
	else if (turn_out == 1)
	{
		printf("Err, extra symbols\n");
		return input_devide_action(devider,action,counts);
	}
	return 0;
}

int check_input_after(void)/*проверка на лишние символы*/
{
	int counter = 0;
	int clean;
	while(((clean = getchar()) != '\n') && (clean != EOF))
	{
		if (clean != ' ' && clean != '\t')
		{
			counter++;
		}
	}
	if (clean == EOF)
	{
		return 2;
	}
	if (counter != 0)
	{
		return 1;
	}
	return 0;
}

int clean_buffer(void)/*очистка ввода и поиск EOF*/
{
	int clean;
	while(((clean = getchar()) != '\n') && (clean != EOF));
	if (clean == EOF)
	{
		return 1;
	}
	return 0;
}

void print_string(char * txt)/*функция для печатки строки*/
{
	int counter = 0;
	while(*(txt + counter) != '\0')
	{
		printf("%c",*(txt + counter));
		counter++;
	}
	printf("\n");
}

char ** devider_string(char * txt, char devider, char *** cleaning)/*разделяем строку на подстроки*/
{
	int counter = 0;   /*двигаемся по полученной строке*/
	int string_devided = 0;  /*двигаемся по массиву указателей которые указывают на полученные разделенные строки*/
	int coef = 2;    /*коэффицент перевыделения памяти*/ 
	int start_position = 0;   /*откуда начинаем копировать строку*/
	int difference;    /*для того чтобы перекопировать строку после последнего разделителя*/
	char ** txt_devided = txt_devided = (char **)malloc(LEN * sizeof(char *));
	if (txt_devided == NULL)
	{
		printf("memory error, ending\n");
		free(cleaning);
		free(txt);
		exit(1);
	}
	/*выделяем память для указателей на разделенные строки*/
	start_position = skip_devider_begin(txt,devider,&counter);   /*пропускаем все разделители в начале*/
	      /*исходной строки*/
	if (start_position == -1)
	{
		free(txt_devided);
		return NULL;
	}    /*если вся строка из разделителей или ни одного разделителя*/
	while (*(txt + counter) != '\0')   /*идем по исходной строке*/
	{
		if (*(txt + counter) == devider)     /*если попали на разделитель*/
		{
			if (string_devided == ((coef - 1) * LEN - 1))    /*проверяем хватает ли паямти для новой разд строки*/
			{                            
				txt_devided = realloc (txt_devided,sizeof(char *) * LEN * coef);
				if (txt_devided == NULL)
				{
					printf("memory error, ending\n");
					free(cleaning);
					free(txt);
					exit(1);
				}
				coef++;
			}
			difference = counter - start_position;
			if (input_string_from_devider(string_devided,txt_devided,txt,&start_position,difference) == 1)
			{
				printf("memory error, ending\n");
				*(txt_devided + string_devided) = NULL;
				counter = 0;
				while (*(txt_devided + counter) != NULL)
				{
					free(*(txt_devided + counter));
					counter++;
				}
				free(cleaning);
				free(txt_devided);
				free(txt);
				exit(1);
			}
			/*получаем очередную разделенную подстроку*/                                           
			string_devided++;     /*увеличваем счетчик*/
			start_position = skip_devider_middle(txt,devider,&counter);     /*пропускае все разделители после*/
			       /*ведь мы уже сейчас на разделителе*/
			if (start_position == -1)
			{
				*(txt_devided + string_devided) = NULL;
				return txt_devided;
			}      /*если после разделителей(-я) ничего нет*/
		}
		counter++;
	}
	if (start_position == 0)
	{
		printf("Current string doesn't have such devider, try again\n");
		free(txt_devided);
		return NULL;
	}    /*если нет ни одного разделителя*/
		 /*если после последнего разделителя есть еще символы*/
	if (string_devided == ((coef - 1) * LEN - 1))    /*проверяем хватает ли на памяти*/
	{
		txt_devided = realloc(txt_devided,coef*LEN*sizeof(char *));
		if (txt_devided == NULL)
		{
			printf("memory error, ending\n");
			free(cleaning);
			free(txt);
			exit(1);
		}
	}
	difference = counter - start_position;       /*сколько осталось символов после последнего разделителя*/
	if (input_string_from_devider(string_devided,txt_devided,txt,&start_position,difference) == 1)
	{
		printf("memory error, ending\n");
		*(txt_devided + string_devided) = NULL;
		counter = 0;
		while (*(txt_devided + counter) != NULL)
		{
			free(*(txt_devided + counter));
			counter++;
		}
		free(txt_devided);
		free(cleaning);
		free(txt);
		exit(1);
	}
	/*получаем последнеюю строчку после разделителя*/
	string_devided++;                                                
	*(txt_devided + string_devided) = NULL;    /*признак конца массива указателей на разделенные подстроки*/
	return txt_devided;       /*возвращаем указатель на массив указателей*/
}

int input_string_from_devider(int string_devided, char ** txt_devided, char * txt, int * start_position,int difference)
{
	*(txt_devided + string_devided) = (char *)malloc(LEN * sizeof(char));   /*выделяем память*/
	if (*(txt_devided + string_devided) == NULL)
	{
		return 1;
	}
	int counter_devided = 0;    /*для похода по подстрочке*/
	int coef = 2;     /*коэф выделения памяти*/
	while (difference != 0)   /*пока не напечатали все нужные символы*/
	{
		*(*(txt_devided + string_devided) + counter_devided) = *(txt + *start_position);                                  
		counter_devided++;   /*присваиваем и увелич счетчик*/
		(*start_position)++;
		if (counter_devided == ((coef - 1) * LEN - 1))   /*если нужно добавляем памяти*/
		{
			*(txt_devided + string_devided) = realloc(*(txt_devided + string_devided),coef * LEN * sizeof(char));
			if (*(txt_devided + string_devided) == NULL)
			{
				return 1;
			}
			coef++;
		}
		difference--;
	}
	(*start_position)++; /*задали новую стартовую позиция для копирования*/
	*(*(txt_devided + string_devided) + counter_devided) = '\0';  /*признак конца подстроки*/
	*(txt_devided + string_devided) = realloc(*(txt_devided + string_devided),(counter_devided + 1) * sizeof(char));
	if (*(txt_devided + string_devided) == NULL)
	{
		return 1;
	}
	return 0;
	/*освобождаем память*/
}

int skip_devider_begin(char * txt, char devider, int * counter)/*пропускаем разделители в начале*/
{
	while (*(txt + *counter) == devider)
	{
		(*counter)++;
	}
	if (*(txt + *counter) == '\0')
	{
		printf("All string is devider, try again\n");
		return -1;
	}
	return (*counter);
}

int skip_devider_middle(char * txt, char devider, int * counter)/*пропускаем разделители в середине строки*/
{
	while (*(txt + *counter) == devider)
	{
		(*counter)++;
		if (*(txt + *counter) == '\0')
		{
			return -1;
		}
	}
	return (*counter);
}

char ** delete_strings(char *** txt_devided, int counts, int length_array)/*удаляем строки*/
{
	int i = 0;
	char ** txt_shorter = (char **)malloc(sizeof(char *) * length_array);
	if (txt_shorter == NULL)
	{
		while (*(*txt_devided + i) != NULL)
		{
			free(*(*txt_devided + i));
			i++;
		}
		return NULL;
	}
	int counter = 0;
	int length;
	int string_added = 0;
	while (*(*txt_devided + counter) != NULL)
	{
		length = str_len(*(*txt_devided + counter));
		if (length <= counts)
		{
			*(txt_shorter + string_added) =  *(*txt_devided + counter);
			string_added++;
		}
		else
		{
			free(*(*txt_devided + counter));
			*(*txt_devided + counter) = NULL;
		}
		counter++;
	}
	*(txt_shorter + string_added) =  NULL;
	free(*txt_devided);
	return txt_shorter;  
}

int str_len(char * txt)/*длина строки*/
{
	int counter = 0;
	while(*(txt + counter) != '\0')
	{
		counter++;
	}
	return counter;                          
}

void sort_by_increase(char ** txt_devided, int length_array)/*сортировка пузырьком для действия 1*/
{
	int i,j;
	int flag;
	char * temp;
	for (i = 0;i < length_array - 1;i++)
	{
		for(j = 0;j < length_array - 1 - i; j++)
		{
			flag = cmp_string(*(txt_devided + j),*(txt_devided + j + 1));
			if (flag == 1)
			{
				temp = *(txt_devided + j +1);
				*(txt_devided + j +1) = *(txt_devided + j);
				*(txt_devided + j) = temp;
			} 
		}
	}
}

void sort_by_decrease(char ** txt_devided, int length_array)/*сортировка пузырьком для действия 2*/
{
	int i,j;
	int flag;
	char * temp;
	for (i = 0;i < length_array - 1;i++)
	{
		for(j = 0;j < length_array - 1 - i; j++)
		{
			flag = cmp_string(*(txt_devided + j),*(txt_devided + j + 1));
			if (flag == 2)
			{
				temp = *(txt_devided + j +1);
				*(txt_devided + j +1) = *(txt_devided + j);
				*(txt_devided + j) = temp;
			} 
		}
	}
}

int cmp_string(char * string1, char * string2)/*сравниваем строки*/
{
	int counter = 0;
	while((*(string1 + counter) != '\0') && (*(string2 + counter) != '\0'))
	{
		if(*(string1 + counter) > *(string2 + counter))
		{
			return 1;
		}
		else if (*(string1 + counter) < *(string2 + counter))
		{
			return 2;
		}
		counter++;
	}
	if (*(string1 + counter) == '\0')
	{
		return 2;
	}
	else if (*(string2 + counter) == '\0')
	{
		return 1;
	}
}

int read_number(char * txt)/*считаем число N с терминала*/
{
	int counter = 0;
	int number = 0;
	while (*(txt + counter) != '\0')
	{
		if (*(txt + counter) >= '0' && *(txt + counter) <= '9')
		{
			number = number * 10 + ((*(txt + counter)) - '0');
		}
		else
		{
			return -1;
		}
		counter++;
	}
	return number;
}