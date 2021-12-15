void free_list_of_commands(char ** argv)
{
    int counter = 0;
    while (argv[counter] != NULL)
    {
        free(argv[counter]);
        argv[counter] = NULL;
        counter++;
    }
    free(argv);
}

void free_input_line(char * line)
{
    free(line);
}

int is_only_spaces(char * line, int position)
{
    while (line[position] == ' ' || line[position] == '\t')
    {
        position++;
    }
    if (line[position] == '\0')
    {
        return 1;
    }
    return 0;
}

int is_there_devider_first(char * line,int position)
{
    while (line[position] == ' ' || line[position] == '\t')
    {
       position++;
    }
    if (line[position] == '|')
    {
        return 1;
    }
    return 0;
}

int skip_to_devider(char * line,int * position)
{
    while (line[*position] != '|')
    {
        if (line[*position] == '\0')
        {
            return 1;
        }
        (*position)++;
    }
    line[*position] = '\0';
    return 0;
}

void skip_to_zero(char * line,int * position)
{
    while (line[*position] != '\0')
    {
        (*position)++;
    }
}

int empty_line(char * line)
{
    int position = 0;
    while (line[position] == ' ' || line[position] == '\t')
    {
        position++;
    }
    if (line[position] == '\0')
    {
        return 1;
    }
    return 0;
}

void skip_spaces(char * line, int * position)
{
    while (line[*position] == ' ' || line[*position] == '\t')
    {
        (*position)++;
    }
}

int check_on_space_or_end(char symbol)
{
    if (symbol == ' ' || symbol == '\t' || symbol == '\0')
    {
        return 0;
    }
    return 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int is_std_1(char p1, char p2)
{
    if (p1 == '>' && p2 == '>')
    {
        return 2;
    }
    if (p1 == '2' && p2 == '>')
    {
        return 2;
    }
    if (p1 == '<')
    {
        return 1;
    }
    if (p1 == '>')
    {
        return 1;
    }
    return 0;
}

int is_std_2(char p1,char p2)
{
    if (p1 == '<' || p1 == '>' || p1 == '\0')
    {
        return -1;
    }
    if (p1 == '2' && p2 == '>')
    {
        return -1;
    }
    return 1;
}

int is_there_std(char * line, int position)
{
    int ans;
    while (line[position] != '\0')
    {   
        if ((ans = is_std_1(line[position],line[position + 1])) != 0)
        {
            if ((ans == 2) && (line[position] == '2'))
            {
                if (line[position - 1] != ' ')
                {
                    if (line[position - 1] != '\t')
                    {
                        position++;
                        continue;
                    }
                }
            }
            if (is_std_2(line[position + ans],line[position + 1 + ans]) == -1)
            {
                return -1;
            }
            return 1;
        }
        position++;
    }
    return 0;
}

int skip_to_std(char * line, int * position)
{
    int ans;
    while (is_std_1(line[*position],line[*position + 1]) == 0)
    {
        if (line[*position] == '\0')
        {
            return 0;
        }
        (*position)++;
    }
    ans = is_std_1(line[*position],line[*position + 1]);
    if ((line[*position + 2] == '>') && line[*position] == '2')
    {
        (*position) += (ans + 1);
        return ans;
    }
    if (line[*position] == '2')
    {
        if (line[*position - 1] != ' ')
        {
            if (line[*position - 1] != '\t')
            {
                ans = 1;
                (*position) += (ans + 1);
                return ans;
            }
        }
    }
    (*position) += ans;
    return ans;
}

int check_smt_after_std(char * line , int position)
{
    while (line[position] == ' ' || line[position] == '\t')
    {
        position++;
    }
    if (line[position] == '\0')
    {
        return 1;
    }
    return 0;
}

int is_std_first(char * line, int * position)
{
    int skip = 0;
    while (line[*position] == ' ' || line[*position] == '\t')
    {
        (*position)++;
    }
    if (is_std_1(line[*position],line[*position + 1]) != 0)
    {
        return 1;
    }
    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void str_copy(char file_name[],char * line,int start,int end)
{
    int counter = 0;
    while(start != (end + 1))
    {
        file_name[counter] = line[start];
        start++;
        counter++;
    }
    file_name[counter] = '\0';
}