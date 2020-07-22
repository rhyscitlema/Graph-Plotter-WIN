/*
	files.c
*/

#include "main.h"
#include <_stdio.h>
#include <files.h>
#include <timer.h>
#include <mouse.h>
#include <tools.h>
#include <userinterface.h>
#include <surface.h>

static const wchar* file_extension=NULL;
static wchar g_file_name[MAX_PATH_LEN+1];
const wchar* get_file_name() { return g_file_name[0] ? g_file_name : NULL; }



static void getFilter (const wchar* file_extension, const wchar** filter, int *filterIndex)
{
	static const wchar* Text = L"Rhyscitlema Function Expression Text (*.rfet)\0*.rfet\0Rhyscitlema Objects Definition Text (*.rodt)\0*.rodt\0Text File (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
	static const wchar* IMAGE = L"Portable Network Graphics (*.png)\0*.png\0All Files (*.*)\0*.*\0";

	int index =
		(0==strcmp21(file_extension, "rfet")) ? 1 :
		(0==strcmp21(file_extension, "rodt")) ? 2 :
		(0==strcmp21(file_extension, "txt" )) ? 3 :

		(0==strcmp21(file_extension, "png" )) ? 1+10 :
		(0==strcmp21(file_extension, "bmp" )) ? 2+10 :
		(0==strcmp21(file_extension, "jpg" )) ? 3+10 :
		(0==strcmp21(file_extension, "jpeg")) ? 4+10 : 0 ;

	if(index==0){
		*filter = Text;
		*filterIndex = 4;
	}
	else if(index < 10){
		*filter = Text;
		*filterIndex = index;
	}
	else{
		*filter = IMAGE;
		*filterIndex = index-10;
	}
}

static OPENFILENAME ofn;

static void Browsefile (HWND hwnd, wchar* file_name, const wchar* file_extension)
{
	const wchar* filter;
	int filterIndex;
	getFilter (file_extension, &filter, &filterIndex);

	memset (&ofn, 0, sizeof(ofn));
	ofn.lStructSize     = sizeof(ofn);
	ofn.hwndOwner       = hwnd;
	ofn.lpstrFile       = file_name;
	ofn.nMaxFile        = MAX_PATH_LEN+1;
	ofn.lpstrFilter     = filter;
	ofn.nFilterIndex    = filterIndex;
	ofn.lpstrFileTitle  = NULL;
	ofn.nMaxFileTitle   = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOREADONLYRETURN;
}

static bool BrowseForOpenfile (HWND hWnd, wchar* file_name, const wchar* file_extension)
{
	Browsefile(hWnd, file_name, file_extension);
	return GetOpenFileName(&ofn);
}

static bool BrowseForSavefile (HWND hWnd, wchar* file_name)
{
	Browsefile(hWnd, file_name, get_extension_from_name(file_name));
	ofn.Flags |= OFN_OVERWRITEPROMPT;
	return GetSaveFileName(&ofn);
}



// save entry text field in a file
bool save_file_as (const wchar* fileName)
{
	wchar filename [MAX_PATH_LEN+1];
	HWND hWnd = hWnd_main_window;

	if(!fileName || !*fileName)
	{
		strcpy22(filename, g_file_name[0] ? g_file_name : L"newfile.txt");
		if(!BrowseForSavefile(hWnd, filename)) return false;
		fileName = filename;
	}
	value stack = stackArray();
	stack = setStr22(stack, userinterface_get_text(UI_MAIN_TEXT));
	stack = FileSave2(fileName, stack);
	if(VERROR(stack))
	{
		MessageBox(hWnd, getMessage(vGetPrev(stack)), L"Error", MB_OK);
		return false;
	}
	if(0!=strcmp22(g_file_name, fileName))
	{
		strcpy22(g_file_name, fileName);
		SetWindowTitle(hWnd, g_file_name);
		file_extension = get_extension_from_name(g_file_name);
	}
	SendMessage(hWnd_main_text, EM_SETMODIFY, false, 0);
	return true;
}

// open a file and load into entry text field
bool open_file (const wchar* fileName)
{
	wchar filename[MAX_PATH_LEN+1];
	HWND hWnd = hWnd_main_window;

	if(!check_save_changes()) return false;

	if(!fileName || !*fileName)
	{
		filename[0] = 0;
		if(!BrowseForOpenfile(hWnd, filename, file_extension)) return false;
		fileName = filename;
	}
	value stack = stackArray();
	if(VERROR(FileOpen2(fileName, stack)))
	{
		MessageBox(hWnd, getMessage(vGet(stack)), L"Error", MB_OK);
		return false;
	}
	// prevent another call to check_save_changes()
	SendMessage (hWnd_main_text, WM_SETTEXT, 0, 0);

	display_main_text(getStr2(vGet(stack)));

	if(0!=strcmp22(g_file_name, fileName))
	{
		strcpy22(g_file_name, fileName);
		SetWindowTitle(hWnd, g_file_name);
		file_extension = get_extension_from_name(g_file_name);
	}
	return true;
}

// save entry text field in currently opened file
bool save_file() { return save_file_as(get_file_name()); }

// reload the currently opened file
bool reload_file() { return open_file(get_file_name()); }

// start a new and empty file entry
bool new_file()
{
	HWND hWnd = hWnd_main_window;
	if(!check_save_changes()) return false;

	file_extension=NULL;
	g_file_name[0]=0;
	SetWindowTitle(hWnd, NULL);

	SendMessage (hWnd_main_text, WM_SETTEXT, 0, 0);
	return true;
}

// notify user if entry text field has been modified
bool check_save_changes ()
{
	if(!SendMessage (hWnd_main_text, EM_GETMODIFY, 0, 0))
		return true; // no changes to save

	const wchar* name = get_name_from_path_name(g_file_name);
	if(!name || !*name) name = L"newfile.txt";

	wchar message[1000+100]; // strlen2(name)+100
	strcpy22(message, L"Do you want to save changes to ");
	strcat22(message, name);
	strcat22(message, L" ?");

	switch(MessageBox (hWnd_main_window, message, L"Save changes?", MB_YESNOCANCEL))
	{
	case IDNO: return true;
	case IDYES: if(save_file()) return true;
	default: SetFocus(hWnd_focused); return false;
	}
}



#ifdef LIBRODT

static wchar* set_rodt_of_objects (wchar* str, List* list)
{
	// go through the list of camera objects
	// start from list_tail() so to resolve
	// container-type dependencies
	Object* object = (Object*)list_tail(list);
	for( ; object != NULL; object = (Object*)list_prev(object))
	{
		if(!object->container) continue;
		// set the RODT of each object
		str = strcpy21(str, "\r\n\\rfet{");
		str = strcpy23(str, c_rfet(object->container));
		str = strcpy21(str, "}\r\n");
	}
	return str;
}

bool save_all_objects ()
{
	wchar fileName[MAX_PATH_LEN+1];
	HWND hWnd = hWnd_main_window;

	if(surface_list()->size==0 && camera_list()->size==0)
	{ MessageBox(hWnd, L"No object to save.", L"Error", MB_OK); return false; }

	value stack = stackArray();
	wchar* str = (wchar*)(stack+2);

	// get the file name to save output to
	strcpy21(fileName, "save_all_objects.rodt");
	if(!BrowseForSavefile (hWnd, fileName)) return false;

	// set the file header
	str = strcpy21(str, "0;\r\n\r\ntype = \"Rhyscitlema_Objects_Definition_Text\" ;\r\n");

	// set the user message
	str = strcpy21(str, "\r\nmessage = \"");
	str = strcpy22(str, userMessage());
	str = strcpy21(str, "\";\r\n");

	str = set_rodt_of_objects (str, camera_list());
	str = set_rodt_of_objects (str, surface_list());

	// now save the output string to file
	stack = setStr22(stack, (wchar*)(stack+2));
	stack = FileSave2(fileName, stack);
	if(VERROR(stack))
	{
		MessageBox(hWnd, getMessage(vGetPrev(stack)), L"Error", MB_OK);
		return false;
	}
	return true;
}



#include <rwif.h>
void take_camera_picture ()
{
	static bool firsttime = true;
	ImageData imagedata;
	Camera* camera;
	wchar filename[MAX_PATH_LEN+1];
	Mouse* mouse = headMouse;
	HWND hWnd = hWnd_main_window;

	camera = mouse->clickedCamera;
	if(camera==NULL || (Object*)camera != mouse->clickedObject)
	{
		MessageBox(hWnd, L"First select the camera to take a picture of.", L"Error", MB_OK);
		return;
	}

	if(firsttime)
	{
		firsttime = false;
		MessageBox (hWnd,
			L"The output file format is\r\n"
			L"specified by the file extension.\r\n"
			L"\r\nIt can be: .png , .jpg or .bmp\n\r", L"Note", MB_OK);
	}
	strcpy21(filename, "picture.png");
	if(!BrowseForSavefile (hWnd, filename)) return;

	imagedata.pixelArray = (unsigned char*)camera->pixelColour;
	imagedata.height = camera->YSize;
	imagedata.width = camera->XSize;
	imagedata.bpp = 4*8;

	wchar message[1000];
	if(write_image_file (C12(filename), &imagedata))
	{
		const_Str2 argv[2] = { L"Successfully saved %s.", get_name_from_path_name(filename) };
		sprintf2(message, 2, argv);
		display_message(message);
	}
	else
	{
		strcpy21(message, rwif_errormessage);
		display_message(message);
		MessageBox(hWnd, message, L"Error", MB_OK);
	}
}

#endif
