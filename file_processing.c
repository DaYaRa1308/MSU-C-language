#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#define LEN 256
#define MAX_LEN_OF_WORD_FATHER 6
#define MAX_LEN_OF_WORD_SON 4

int check_config_file(char *,char **);
int in_list(char **, char); 
int check(char); 
int skip_word(int,int *);                                          

int main(int argc, char * argv[])
{
    pid_t pid;
    int son_error = 0;
    int flag_error = 0;
    int counter = 0;
    int i;
    int status;
    int lenght_word;
    int rez;
    int flag_word = 0;
    int fd;
    char * symbol;
    int amount_of_processes;
    char ** list_of_changes;
    int copy;
    int flag_EOF = 0;
    symbol = (char *)malloc(MAX_LEN_OF_WORD_FATHER * sizeof(char));
    if (symbol == NULL)
    {
        printf("mem err ending\n");
        return 1;
    }
    list_of_changes =(char **)malloc(LEN * sizeof(char *));
    if (list_of_changes == NULL)
    {
        printf("mem err ending\n");
        free(symbol);
        return 1;
    }
    *(list_of_changes) = NULL;
    if (argc != 3)
    {
        printf("err input in amount in terminal, ending\n");
        free(symbol);
        free(list_of_changes);
        return 1;
    }
    if ((amount_of_processes = check_config_file(argv[2],list_of_changes)) == -1)
    {
        free(symbol);
        free(list_of_changes);
        return 1;
    }
    if (amount_of_processes == 0)
    {
        printf("nothing to do ending\n");
        free(symbol);
        free(list_of_changes);
        return 1;
    }
    if (*(list_of_changes) == NULL)
    {
        printf("there are no comb(letter_old letter_new) ending\n");
        free(list_of_changes);
        free(symbol);
        return 0;
    }
    while (counter != (amount_of_processes - 1))
    {
        if ((pid = fork()) > 0)
        {
            counter++;
            continue;
        }
        else if (pid == 0)
        {
            flag_error = 0;
            counter++;
            flag_EOF = 0;
            fd = open(argv[1],O_RDWR);
            if (fd == -1)
            {
                printf("err in son:%d\n",counter);
                perror("Код ошибки:");
                printf("\n");
                free(symbol);
                counter = 0;
                while (*(list_of_changes + counter) != NULL)
                {
                    free(*(list_of_changes + counter));
                    counter++;
                }
                free(list_of_changes);
                exit(1);
            }
            while ((rez = read(fd,symbol,sizeof(char))) != 0)
            {
                if (rez == -1)
                {
                    continue;
                }
                if (check(*symbol) == 1)
                {
                    continue;
                }
                if (check(*symbol) == 0)
                {
                    rez = in_list(list_of_changes,*symbol);
                    if (rez != counter)
                    {
                        lenght_word = skip_word(fd,&flag_EOF);
                        if (lenght_word == -1)
                        {
                            printf("err in son:%d\n",counter);
                            perror("Код ошибки:");
                            printf("\n");
                            flag_error = 1;
                            break;
                        }
                        if (flag_EOF == 1)
                        {
                            break;
                        }
                        continue;
                    }
                    else 
                    {
                        lenght_word = skip_word(fd,&flag_EOF);
                        if (lenght_word == -1)
                        {
                            printf("err in son:%d\n",counter);
                            perror("Код ошибки:");
                            printf("\n");
                            flag_error = 1;
                            break;                                      
                        }
                        if (lenght_word < 10)
                        {
                            if (flag_EOF == 1)
                            {
                                break;
                            }
                            else
                            {
                                while (lseek(fd,(-1) * sizeof(char),SEEK_CUR) == -1);
                                continue;
                            }
                        }
                        else
                        {
                            while (lseek(fd,(-1)*(lenght_word + 1) * sizeof(char),SEEK_CUR) == -1);
                            *symbol = '*';
                            *(symbol + 1) = *(*(list_of_changes + counter) + 1);
                            rez = 0;
                            copy = 0;
                            while (rez != 2)
                            {
                                copy = rez;
                                rez += write(fd,symbol + rez,(2 - rez) * sizeof(char));
                                if (rez == (copy - 1))
                                {
                                    rez++;
                                    continue;
                                }
                            }
                            if (counter < 10)
                            {
                                while (lseek(fd,(lenght_word - 6) * sizeof(char),SEEK_CUR) == -1);
                                *symbol = 'S';
                                *(symbol + 1) = 'O';
                                *(symbol + 2) = 'N';
                                *(symbol + 3) = '0' + counter;
                                rez = 0;
                                copy = 0;
                                while (rez != MAX_LEN_OF_WORD_SON)
                                {
                                    copy = rez;
                                    rez += write(fd,symbol + rez,(4 - rez) * sizeof(char));
                                    if (rez == (copy - 1))
                                    {
                                        rez++;
                                        continue;
                                    }
                                }
                            }
                            else if (counter >= 10 && counter < 100)
                            {
                                while (lseek(fd,(lenght_word - 7) * sizeof(char),SEEK_CUR) == -1);
                                *symbol = 'S';
                                *(symbol + 1) = 'O';
                                *(symbol + 2) = 'N';
                                *(symbol + 3) = '0' + (counter / 10);
                                *(symbol + 4) = '0' + (counter % 10);
                                rez = 0;
                                copy = 0;
                                while (rez != MAX_LEN_OF_WORD_SON + 1)
                                {
                                    copy = rez;
                                    rez += write(fd,symbol + rez,(5 - rez) * sizeof(char));
                                    if (rez == (copy - 1))
                                    {
                                        rez++;
                                        continue;
                                    }
                                }
                            }
                            else
                            {
                                while (lseek(fd,(lenght_word - 8) * sizeof(char),SEEK_CUR) == -1);
                                *symbol = 'S';
                                *(symbol + 1) = 'O';
                                *(symbol + 2) = 'N';
                                *(symbol + 3) = '0' + (counter / 100);
                                *(symbol + 4) = '0' + ((counter / 10) % 10);
                                *(symbol + 5) = '0' + (counter % 100);
                                rez = 0;
                                copy = 0;
                                while (rez != MAX_LEN_OF_WORD_SON + 2)
                                {
                                    copy = rez;
                                    rez += write(fd,symbol + rez,(6 - rez) * sizeof(char));
                                    if (rez == (copy - 1))
                                    {
                                        rez++;
                                        continue;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            close(fd);
            free(symbol);
            counter = 0;
            while (*(list_of_changes + counter) != NULL)
            {
                free(*(list_of_changes + counter));
                counter++;
            }
            free(list_of_changes);
            if (flag_error == 1)
            {
                exit(1);
            }
            exit(0);
        }
        else
        {
            counter++;
            son_error++;
            printf("err in process creation\n");
            printf("Son number:%d won't work\n",counter);
            perror("Код ошибки:");
            printf("\n");
        } 
    }
    copy = 0;
    flag_error = 0;
    counter = 0;
    flag_EOF = 0;
    fd = open(argv[1],O_RDWR);
    if (fd == -1)
    {
        printf("err in father\n");
        perror("Код ошибки:");
        printf("\n");
        free(symbol);
        counter = 0;
        while (*(list_of_changes + counter) != NULL)
        {
            free(*(list_of_changes + counter));
            counter++;
        }
        free(list_of_changes);
        printf("complete running with errors\n");
        for (i = 0; i < (amount_of_processes - 1 - son_error); i++)
        {
            wait(&status);
        }
        return 1;
    }
    while ((rez = read(fd,symbol,sizeof(char))) != 0)
    {
        if (rez == -1)
        {
            continue;
        }
        if (check(*symbol) == 1)
        {
            continue;
        }
        if (check(*symbol) == 0)
        {
            rez = in_list(list_of_changes,*symbol);
            if (rez != counter)
            {
                lenght_word = skip_word(fd,&flag_EOF);
                if (lenght_word == -1)
                {
                    printf("err in father\n");
                    perror("Код ошибки:");
                    printf("\n");
                    flag_error = 1;
                    break;
                }
                if (flag_EOF == 1)
                {
                    break;
                }
                continue;
            }
            else 
            {
                lenght_word = skip_word(fd,&flag_EOF);
                if (lenght_word == -1)
                {
                    printf("err in father\n");
                    perror("Код ошибки:");
                    printf("\n");
                    flag_error = 1;
                    break;
                }
                if (lenght_word < 10)
                {
                    if (flag_EOF == 1)
                    {
                        break;
                    }
                    else
                    {
                        while (lseek(fd,(-1) * sizeof(char),SEEK_CUR) == -1);
                        continue;
                    }
                }
                else
                {
                    while (lseek(fd,(-1)*(lenght_word + 1) * sizeof(char),SEEK_CUR) == -1);
                    *symbol = '*';
                    *(symbol + 1) = *(*(list_of_changes) + 1);
                    rez = 0;
                    copy = 0;
                    while (rez != 2)
                    {
                        copy = rez;
                        rez += write(fd,symbol + rez,(2 - rez) * sizeof(char));
                        if (rez == (copy - 1))
                        {
                            rez++;
                            continue;
                        }
                    }
                    while (lseek(fd,(lenght_word - 8) * sizeof(char),SEEK_CUR) == -1);
                    *symbol = 'F';
                    *(symbol + 1) = 'A';
                    *(symbol + 2) = 'T';
                    *(symbol + 3) = 'H';
                    *(symbol + 4) = 'E';
                    *(symbol + 5) = 'R';
                    rez = 0;
                    copy = 0;
                    while (rez != MAX_LEN_OF_WORD_FATHER)
                    {
                        copy = rez;
                        rez += write(fd,symbol + rez,(6 - rez) * sizeof(char));
                        if (rez == (copy - 1))
                        {
                            rez++;
                            continue;
                        }
                    }
                }
            }
        }
    }
    for (i = 0; i < (amount_of_processes - 1 - son_error); i++)
    {
        wait(&status);
    }
    close(fd);
    free(symbol);
    counter = 0;
    while (*(list_of_changes + counter) != NULL)
    {
        free(*(list_of_changes + counter));
        counter++;
    }
    free(list_of_changes);
    printf("complete running\n");
    if (flag_error == 1)
    {
        return 1;
    }
    return 0;
}

int skip_word(int fd,int * flag_EOF)
{
    int rez;
    int counter = 0;
    char symbol = '1';
    while(check(symbol) == 0)
    {
        while ((rez = read(fd,&symbol,sizeof(char))) == -1);
        if (rez == 0)
        {
            *flag_EOF = 1;
            return counter;
        }
        counter++;
    }
    return counter;
}

int in_list(char ** list_of_changes, char symbol)
{
    int counter = 0;
    while (*(list_of_changes + counter) != NULL)
    {
        if (**(list_of_changes + counter) == symbol)
        {
            return counter;
        }
        counter++;
    }
    return -1;
}

int check_config_file(char * file_name, char ** list_of_changes)
{
    int flag = 1;
    int amount_of_processes = 0;
    int counts;
    char symbol;
    int symbol_pair;
    char symbols[LEN];
    int i;
    int counter = 0;
    int read;
    FILE * fp = fopen(file_name,"r");
    for (i = 0; i < LEN; i++)
    {
        symbols[i] = 0;
    }
    if (fp == NULL)
    {
        printf("Err such config file doesn't exist ending\n");
        return -1;
    }
    while((read = getc(fp)) == ' ' || read == '\t');
    if (read == EOF || read == '\n')
    {
        printf("err can't find amount of processes ending\n");
        fclose(fp);
        return -1;
    }
    if (read >= '0' && read <= '9')
    {
        amount_of_processes *= 10;
        amount_of_processes += read - '0';
        while ((read = getc(fp)) != ' ' && read != EOF && read != '\n' && read != '\t')
        {
            if (read >= '0' && read <= '9')
            {
                amount_of_processes *= 10;
                amount_of_processes += read - '0';
            } 
            else 
            {
                printf("err in amount of processes ending\n");
                fclose(fp);
                return -1;
            }
        }
        if (read == EOF)
        {
            return 0;
        }
        if (read == ' ' || read == '\t')
        {
            while ((read = getc(fp)) == ' ' || read == '\t');
            if (read != '\n')
            {
                printf("err in amount of processes ending\n");
                fclose(fp);
                return -1;
            }
        }
    }
    else 
    {
        printf("err in amount of processes ending\n");
        fclose(fp);
        return -1;
    }
    if (amount_of_processes <= 0)
    {
        printf("err in amount of processes ending\n");
        fclose(fp);
        return -1;
    }
    counts = amount_of_processes;
    while (flag)
    {
        if (fscanf(fp," %c",&symbol) == 1)
        {
            if (symbol == '*')
            {
                printf("get * ending\n");
                counter = 0;
                while (*(list_of_changes + counter) != NULL)
                {
                    free(*(list_of_changes + counter));
                    counter++;
                }
                fclose(fp);
                return -1;
            }
            counts--;
            if (counts == -1)
            {
                printf("get more comb(letter_old letter_new)\n");
                printf("cut down further\n");
                break;
            }
            if (symbols[symbol] == 0)
            {
                while ((symbol_pair = getc(fp)) == ' ' || symbol_pair == '\t');
                if (symbol_pair != EOF && check((char)symbol_pair) == 0)
                {
                    symbols[symbol] = symbol_pair;
                    *(list_of_changes + counter) = (char *)malloc(2 * sizeof(char));
                    if (*(list_of_changes + counter) == NULL)
                    {
                        counter = 0;
                        while (*(list_of_changes + counter) != NULL)
                        {
                            free(*(list_of_changes + counter));
                            counter++;
                        }
                        printf("mem err ending\n");
                        fclose(fp);
                        return -1;
                    }
                    **(list_of_changes + counter) = symbol;
                    *(*(list_of_changes + counter) + 1) = (char)symbol_pair;
                    counter++;
                    *(list_of_changes + counter) = NULL;

                }
                else
                {
                    printf("Err with pair symbol ending\n");
                    counter = 0;
                    while (*(list_of_changes + counter) != NULL)
                    {
                        free(*(list_of_changes + counter));
                        counter++;
                    }
                    fclose(fp);
                    return -1;
                }
            }
            else
            {
                printf("Err has got already entered symbol ending\n");
                fclose(fp);
                return -1;
            }
        }
        else
        {
            break;
        }
    }
    if (counts == amount_of_processes)
    {
        fclose(fp);
        return 0;
    }
    else if (counts >0)
    {
        printf("get more amount of processes\n");
        printf("cut down amount of processes\n");
        amount_of_processes = amount_of_processes - counts;
        printf("new amount:%d\n",amount_of_processes);
    }
    fclose(fp);
    return amount_of_processes;
}

int check(char symbol)
{
    if (symbol == ' ' || symbol == ',' || symbol == '.' || symbol == '\n' || symbol == '\t')
    {
        return 1;    
    } 
    return 0;
}  