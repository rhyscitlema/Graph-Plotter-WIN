/*
    dialog_boxes.h
*/
#ifndef _DIALOG_BOXES_H
#define _DIALOG_BOXES_H

#include "main.h"
#include "resource.h"

extern HWND hWnd_find;
extern HWND hWnd_repl;

extern bool find_dialog_box ();
extern bool repl_dialog_box ();

extern bool goto_dialog_box (HWND hWnd);

extern bool print_dialog_box (HWND hWnd);

extern bool about_dialog_box (HWND hWnd);

extern bool create_dialog_box (
    HWND hWnd, int IDD,     // provide IDentity of Dialog box
    void (*user_entry_get) (wchar* entry),  // provide initial entry value
    bool (*user_entry_apply) (value stack, const wchar* entry)); // return error message

#endif
