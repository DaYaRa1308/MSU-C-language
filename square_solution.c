#include <stdio.h>
#include <math.h>

int flag = 1;/*y или n*/

int solve(float,float,float);
int repeat(void);
int input_coef(float *,float *,float *);
int check_input_after();
int clean_buffer();
int main(void)
{
	extern int flag;
	float a,b,c;/*коэфиценты уравнения*/
	while(flag == 1)
	{	
		if (input_coef(&a,&b,&c) == 1)
		{
			printf("See EOF, ending\n");
			return 0;                   
		}
		solve(a,b,c);               
		printf("One more?: y-yes n-no\n");
		if (repeat() == 1)
		{
			printf("See EOF, ending\n");
			return 0; 
		}
	}
	return 0;
}

int repeat()/*ввод да или нет*/
{
	extern int flag;
	int ans;
	int turn_out;
	int run = 1;
	while(run)
	{
		while(((ans = getchar()) == ' ') || (ans == '\n') || (ans == '\t'))
		{
			if (ans == EOF)
			{
				return 1;
			}
		}
		turn_out = check_input_after();
		if (turn_out == 2)
		{
			return 1;
		}
		else if(turn_out == 1)
		{
			printf("ERR, try again\n");
			return repeat();
		}
		else
		{
			run = 0;
		}
	}
	if (ans == 'y')
	{}
	else if (ans == 'n')
	{
		flag = 0;
	}
	else
	{
		printf("ERR, try again\n");
		return repeat();
	}
	return 0;
}

int solve(float a,float b,float c)/*Решаем кв уравнение математикой 8 класса)))*/
{
	double d;/*дискриминант*/
	double ans1, ans2;/*решения*/
	if (a == 0 && b != 0)
	{
		printf("Solution:%f\n",-(c/b));
	}
	else if (a == 0 && b == 0)
	{
		if(c == 0)
		{
			printf("Infinity many solutions\n");
			return 0;
		}
		else
		{
			printf("Zero solutions\n");
			return 0;
		}
	}
	else
	{
		d = pow(b,2)-4*a*c;
		if(d < 0)
		{
			printf("Zero solutions\n");
			return 0;
		}
		else if(d == 0.0)
		{
			ans1 = (-b)/(2*a);
			printf("Solution:%f\n",ans1);
			return 0;
		}
		else
		{
			ans1 = (-b - pow(d,0.5))/(2*a);
			ans2 = (-b + pow(d,0.5))/(2*a);
			printf("Solutions:%f %f\n",ans1, ans2);
			return 0;
		}
	}
}
int input_coef(float * a,float * b,float * c)/*вводим коэффиценты*/
{
	int turn_out;
	printf("Input 3 coefs\n");
	while(scanf(" %f %f %f",a,b,c) != 3)/*ошибка ввода коэффицентов*/
	{
		printf("ERR,try again\n");
		if (clean_buffer() == 1)
		{
			return 1;
		}
	}
	turn_out = check_input_after();
	if (turn_out == 2)
	{
		return 1;
	}
	else if(turn_out == 1)
	{
		printf("More than 3 coefs, try again\n");
		return input_coef(a,b,c);
	}
	return 0;
}
int check_input_after()/*проверяем чтоб после удачного ввода не было лишних символов*/
{
	int counter = 0;/*подсчет символов после*/
    int clean;/*чистка ввода*/
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
	{/*лишние символы заново вводим*/
		return 1;
	}
	return 0;
}

int clean_buffer()/*для очитски потока ввода*/
{
	int e = 0;
	while((e = getchar()) != '\n' && e != EOF);
	if (e == EOF)
	{
		return 1;
	}
	return 0;
}