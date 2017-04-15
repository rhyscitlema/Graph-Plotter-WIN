/*
    font.h
*/
#ifndef _FONT_H
#define _FONT_H


#include "main.h"

extern void font_initialise ();         // set initial font on text fields

extern void font_remove ();             // delete objects created by font

extern bool font_dialog_box (HWND hWnd); // show dialog box, return TRUE if font changed

extern bool font_pcn_to_chr (HWND hWnd);

extern bool font_chr_to_pcn (HWND hWnd);

extern bool font_chr_to_fcn (HWND hWnd);

extern bool font_set_pif_cn (HWND hWnd);

#endif
