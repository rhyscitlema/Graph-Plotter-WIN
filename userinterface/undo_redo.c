/*
    undo_redo.c

    *** quick-and-dirty work done here !!!
*/

#include "undo_redo.h"


typedef struct _UNDOREDO
{
    HWND hWnd;
    int value;
} UNDOREDO;

UNDOREDO Array [100];
int total=0;


int set_undo (HWND hWnd, int value)
{
    int i;
    for(i=0; i < total; i++)
        if(hWnd == Array[i].hWnd)
        {
            Array[i].value = value;
            return 1;
        }
    Array[total].hWnd = hWnd;
    Array[total].value = value;
    total++;
    return 0;
}


int get_undo (HWND hWnd)
{
    int i;
    for(i=0; i < total; i++)
        if(hWnd == Array[i].hWnd)
            return Array[i].value;
    return 0;
}
