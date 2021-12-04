#include <stdio.h>
#include <math.h>

enum 
{
	action_0 = '0',
	action_1 = '1',
	action_2 = '2',
	action_3 = '3',
	action_4 = '4',
	action_5 = '5',
	action_6 = '6',
	action_7 = '7'
};
enum
{
	JAN = 1,
	FEB = 2,
	MAR = 3,
	APR = 4,
	MAY = 5,
	JUN = 6,
	JUL = 7,
	AUG = 8,
	SEP = 9,
	OCT = 10,
	NOV = 11,
	DEC = 12
};
#define DEG_YEAR 7
#define DEG_MONTH 4
#define DEG_DAY 5
#define BASE 2
#define MASK_DAY 0xF800
#define MASK_MONTH 0x780
#define MASK_YEAR 0x7F
#define DAY_MAX 31
#define MONTH_MAX 12
#define YEAR_MAX 99

int check_date(unsigned short, unsigned short, unsigned short);
int input_date(unsigned short *, unsigned short *, unsigned short*);
int check_input_after(void);
int clean_buffer(void);
void package(unsigned short,unsigned short,unsigned short,unsigned short *);
void reverse_package(unsigned short *, unsigned short *, unsigned short *, unsigned short);
void write_package_date(unsigned short);
void write_only_day(unsigned short);
void write_only_month(unsigned short);
void write_only_year(unsigned short);
int change_day(unsigned short *);
int change_month(unsigned short *);
int change_year(unsigned short *);
void write_package_formate(unsigned short);

int main(void)
{
	unsigned short date;/*упаковыввем дату сюда*/
	unsigned short day,month,year;/*вводим дату сюда*/
	char check = -1;/*для цикла 0-7*/
	int run = 1;
	if (input_date(&day, &month, &year) == 1)
	{
		printf("See EOF, ending");   
		return 0;                                                      
	}
	package(day,month,year,&date);
	printf("Set number: 0-7\n");           
	printf("0-Print date\n");
	printf("1-Print day\n");
	printf("2-Print month\n");
	printf("3-Print year in from 19/20 ГГ\n");
	printf("4-Change day?\n");
	printf("5-Change month?\n");
	printf("6-Change year?\n");
	printf("7-Print binary form of date\n");
	scanf(" %c",&check);
	while(run)/*выпоплнение действий пользователя*/
	{
		if (check_input_after() != 0)
		{
			printf("Unexpected inpud, exit\n");
			return 0;
		}
		switch(check)
		{
			case action_0:                    
						write_package_date(date);                                   
						break;
			case action_1:  
						write_only_day(date);                  
						break;
			case action_2:
						write_only_month(date);
						break;
			case action_3:
						write_only_year(date);
						break;
			case action_4:
						if (change_day(&date) == 1)
						{
							printf("See EOF, ending");   
							return 0; 
						}
						break;
			case action_5:
						if (change_month(&date) == 1)
						{
							printf("See EOF, ending");   
							return 0; 
						}
						break;
			case action_6:
						if (change_year(&date) == 1)
						{
							printf("See EOF, ending");   
							return 0; 
						}
						break;
			case action_7:
						write_package_formate(date);
						printf("\n");
						break;
			default:
					printf("Unexpected input, exit\n");
					return 0;
		}
		printf("More?\n");
		scanf(" %c",&check);
	}
	return 0;
}

int input_date(unsigned short * day, unsigned short * month, unsigned short * year)/*ввода даты*/
{
	int turn_out;
	printf("Set data formate: ЧЧ ММ ГГ\n");
	unsigned int day_copy,month_copy,year_copy;
	while ((scanf(" %u %u %u",&day_copy,&month_copy,&year_copy) != 3) || (day_copy>DAY_MAX) || (month_copy>MONTH_MAX) || (year_copy>YEAR_MAX))
	{
		printf("Err, try again\n");
		if (clean_buffer() == 1)
		{
			return 1;
		}
	}
	*day = day_copy;
	*month = month_copy;
	*year = year_copy;                            
	turn_out = check_input_after();
	if (turn_out == 2)
	{
		return 1;
	}
	else if (turn_out == 1)
	{
		printf("Err extra symbols\n");
		return input_date(day,month,year);
	}
	if (check_date(*day,*month,*year) == 1)
	{
		return input_date(day,month,year);
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

int check_date(unsigned short day, unsigned short month, unsigned short year)/*проверка корректности даты*/
{
	if ((year >= 0) && (year <= 99))
	{
		if ((month >= 1) && (month <= 12))
		{
			switch(month)
			{
				case JAN:
				case MAR:
				case MAY:
				case JUL:
				case OCT:
				case DEC:
						if ((day >= 1) && (day <= 31))
						{
							return 0;
						}
						else
						{
							printf("Err date formate\n");
							return 1;	
						}
						break;
				case APR:
				case JUN:
				case SEP:
				case NOV:
						if ((day >= 1) && (day <= 30))
						{
							return 0;
						}
						else
						{
							printf("Err date formate\n");
							return 1;	
						}
						break;
				case FEB:
						if ((year % 4) == 0)
						{
							if ((day >= 1) && (day <= 29))
							{
								return 0;
							}
							else
							{
								printf("Err date formate\n");
								return 1;		
							}
						}
						else
						{
							if ((day >= 1) && (day <= 28))
							{
								return 0;
							}
							else
							{
								printf("Err date formate\n");
								return 1;		
							}
						}	
						break;
				default:
						break;
			}
		}
		else
		{
			printf("Err date formate\n");
			return 1;
		}
	}
	else
	{
		printf("Err date formate\n");
		return 1;
	}
}

int clean_buffer(void)/*для очитски потока ввода*/
{
	int clean;
	while((clean = getchar()) != '\n' && clean != EOF);
	if (clean == EOF)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void package(unsigned short day,unsigned short month,unsigned short year,unsigned short * date)/*укомплектовывваем дату в 2 байта*/
{
	month = month << DEG_YEAR;
	day = day << (DEG_YEAR + DEG_MONTH);
	*date = year | month | day;
}

void reverse_package(unsigned short * day,unsigned short * month,unsigned short * year,unsigned short date)/*из компактного вида*/
{
	*day = (date & MASK_DAY) >> (DEG_YEAR + DEG_MONTH);
	*month = (date & MASK_MONTH) >> DEG_YEAR;
	*year = date & MASK_YEAR;
}

void write_package_date(unsigned short date)
{
	unsigned short day,month,year;
	reverse_package(&day,&month,&year,date);
	printf("ЧЧ = %hu, ММ = %hu, ГГ = %hu\n",day,month,year);
}

void write_only_day(unsigned short date)
{
	date = date >> (DEG_YEAR + DEG_MONTH);
	printf("ЧЧ = %hu\n",date);
}

void write_only_month(unsigned short date)
{
	date = date & MASK_MONTH;
	date = date >> DEG_YEAR;
	printf("MM = %hu\n",date);
}

void write_only_year(unsigned short date)
{
	date = date & MASK_YEAR;
	if (date <=10)
	{
		printf("ГГ = 200%hu\n",date);
	}
	else if (date <= 49)
	{
		printf("ГГ = 20%hu\n",date);
	}
	else
	{
		printf("ГГ = 19%hu\n",date);
	}
}

int change_day(unsigned short * date)
{
	unsigned int day_copy;
	unsigned short day,month,year;
	int turn_out;
	reverse_package(&day,&month,&year,*date);
	while ((scanf(" %u",&day_copy) != 1) || (day_copy>DAY_MAX) || (day_copy == 0))
	{
		printf("Err, try again\n");
		if (clean_buffer() == 1)
		{
			return 1;
		}
	}
	day = day_copy;
	turn_out = check_input_after();
	if (turn_out == 2)
	{
		return 1;
	}
	else if (turn_out == 1)
	{
		printf("Err extra symbols\n");
		return change_day(date);
	}

	if (check_date(day,month,year) == 1)
	{
		return change_day(date);
	}
	day = day << (DEG_YEAR + DEG_MONTH);
	*date = *date & (MASK_YEAR + MASK_MONTH);
	*date = *date | day; 
	return 0;                            
}

int change_month(unsigned short * date)
{
	unsigned int month_copy;
	unsigned short day,month,year;
	int turn_out;
	reverse_package(&day,&month,&year,*date);
	while ((scanf(" %u",&month_copy) != 1) || (month_copy>MONTH_MAX) || (month_copy == 0))
	{
		printf("Err, try again\n");
		if (clean_buffer() == 1)
		{
			return 1;
		}
	}
	month = month_copy;
	turn_out = check_input_after();
	if (turn_out == 2)
	{
		return 1;
	}
	else if (turn_out == 1)
	{
		printf("Err extra symbols\n");
		return change_month(date);
	}

	if (check_date(day,month,year) == 1)
	{
		return change_month(date);
	}
	month = month << DEG_YEAR;
	*date = *date & (MASK_YEAR + MASK_DAY);
	*date = *date | month; 
	return 0; 
}

int change_year(unsigned short * date)
{
	unsigned int year_copy;
	unsigned short day,month,year;
	int turn_out;
	reverse_package(&day,&month,&year,*date);
	while ((scanf(" %u",&year_copy) != 1) || (year_copy>YEAR_MAX))
	{
		printf("Err, try again\n");
		if (clean_buffer() == 1)
		{
			return 1;
		}
	}
	year = year_copy;
	turn_out = check_input_after();
	if (turn_out == 2)
	{
		return 1;
	}
	else if (turn_out == 1)
	{
		printf("Err extra symbols\n");
		return change_year(date);
	}

	if (check_date(day,month,year) == 1)
	{
		return change_year(date);
	}
	*date = *date & (MASK_DAY + MASK_MONTH);
	*date = *date | year; 
	return 0; 
}

void write_package_formate(unsigned short date)
{
	if (date == 1)
	{
		printf("1");
	}
	else
	{
		if ((date % BASE) == 1)
		{
			write_package_formate(date / 2);
			printf("1");
		}
		else
		{
			write_package_formate(date / 2);
			printf("0");
		}
	}
}