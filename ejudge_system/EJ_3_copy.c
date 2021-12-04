#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void process(struct List *pl, const char *str)
{
    if (pl->first == NULL || pl->last == NULL)
    {
        return;
    }
    struct Node * temp;
    struct Node * pointer  = pl->first;
    struct Node * pointer_last = pl->last;
    while (pointer != pointer_last)
    {
        if (strcmp(pointer->elem,str) == 0)
        {
            free(pointer->elem);
            pointer->elem = NULL;
            if (pointer->prev == NULL)
            {
                pl->first = pointer->next;
                free(pointer);
                pl->first->prev = NULL;
                pointer = pl->first;
            }
            else
            {
                temp = pointer->next;
                pointer->next->prev = pointer->prev;
                pointer->prev->next = pointer->next;
                free(pointer);
                pointer = temp;
            }
        }
        else if (strcmp(pointer->elem,str) > 0)
        {
            if (pointer->prev == NULL)
            {
                pl->first = pointer->next;
                pl->first->prev = NULL;
                pl->last->next = pointer;
                pointer->prev = pl->last;
                pointer->next = NULL;
                pl->last = pointer;
                pointer = pl->first;   
            }
            else
            {
                temp = pointer->next;
                pointer->next->prev = pointer->prev;
                pointer->prev->next = pointer->next;
                pl->last->next = pointer;
                pointer->prev = pl->last;
                pointer->next = NULL;
                pl->last = pointer;
                pointer = temp;
            }
        }
        else
        {
            pointer = pointer->next;
        }
    }
    if (strcmp(pointer->elem,str) == 0)
    {
        free(pointer->elem);
        pointer->elem = NULL;
        if (pointer->next == NULL && pointer->prev == NULL)
        {
            pl->last = NULL;
            pl->first = NULL;
            free(pointer);
            return;
        }
        else if (pointer->prev == NULL)
        {
            pl->first = pl->first->next;
            free(pointer);
            pl->first->prev = NULL;
        }
        else if (pointer->next == NULL)
        {
            pl->last = pl->last->prev;
            free(pointer);
            pl->last->next = NULL;
            return;
        }
        else
        {
            pointer->next->prev = pointer->prev;
            pointer->prev->next = pointer->next;
            free(pointer);
        }
    }
    else if (strcmp(pointer->elem,str) > 0)
    {
        if (pointer->next == NULL && pointer->prev == NULL)
        {
            return;
        }
        else if (pointer->next == NULL)
        {
            return;
        }
        else if (pointer->prev == NULL)
        {
            pl->first = pl->first->next;
            pl->first->prev = NULL;
            pl->last->next = pointer;
            pointer->prev = pl->last;
            pointer->next = NULL;
            pl->last = pointer;  
        }
        else
        {
            pointer->next->prev = pointer->prev;
            pointer->prev->next = pointer->next;
            pl->last->next = pointer;
            pointer->prev = pl->last;
            pointer->next = NULL;
        }
    }
}