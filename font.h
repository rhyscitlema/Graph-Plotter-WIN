/*
	font.h
*/
#ifndef _FONT_H
#define _FONT_H


#include "main.h"

extern void font_initialise ();          // set initial font on text fields

extern void font_remove ();              // delete objects created by font

extern bool font_dialog_box (HWND hWnd); // show dialog box, return TRUE if font changed

extern bool convert_text (HWND hWnd, int ID); // TODO: move this function to somewhere else

#endif
