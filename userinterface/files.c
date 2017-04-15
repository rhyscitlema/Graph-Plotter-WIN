/*
    files.c
*/

#include "main.h"
#include "undo_redo.h"
#include <_stdio.h>
#include <files.h>
#include <timer.h>
#include <mouse.h>
#include <tools.h>
#include <userinterface.h>
#include <surface.h>

#define errormessage errorMessage()



static mchar file_name [MAX_FILE_NAME];
static bool file_exists = false;

const mchar* file_name_get() { return file_name; }
bool file_exists_get() { return file_exists; }



static mchar file_extension[10];

static void getFilter (const mchar* file_extension, const mchar** filter, int *filterIndex)
{
    static const mchar* TEXT = L"Rhyscitlema Object Definition Text (*.rodt)\0*.rodt\0Math Function Expression Text (*.mfet)\0*.mfet\0Text File (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    static const mchar* IMAGE = L"Portable Network Graphics (*.png)\0*.png\0All Files (*.*)\0*.*\0";

    int index =
        (0==strcmp21(file_extension, "rodt")) ? 1 :
        (0==strcmp21(file_extension, "mfet")) ? 2 :
        (0==strcmp21(file_extension, "txt" )) ? 3 :

        (0==strcmp21(file_extension, "png" )) ? 1+10 :
        (0==strcmp21(file_extension, "bmp" )) ? 2+10 :
        (0==strcmp21(file_extension, "jpg" )) ? 3+10 :
        (0==strcmp21(file_extension, "jpeg")) ? 4+10 : 0 ;

    if(index==0)
    {   *filterIndex = 4;
        *filter = TEXT;
    }
    else if(index < 10)
    {   *filter = TEXT;
        *filterIndex = index;
    }
    else
    {   *filterIndex = index-10;
        *filter = IMAGE;
    }
}



static OPENFILENAME ofn;

static void Browsefile (HWND hwnd, mchar* file_name, const mchar* file_extension)
{
    const mchar* filter;
    int filterIndex;
    getFilter (file_extension, &filter, &filterIndex);

    memset (&ofn, 0, sizeof(ofn));
    ofn.lStructSize     = sizeof(ofn);
    ofn.hwndOwner       = hwnd;
    ofn.lpstrFile       = file_name;
    ofn.nMaxFile        = MAX_FILE_NAME;
    ofn.lpstrFilter     = filter;
    ofn.nFilterIndex    = filterIndex;
    ofn.lpstrFileTitle  = NULL;
    ofn.nMaxFileTitle   = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOREADONLYRETURN;
}

static bool BrowseForOpenfile (HWND hWnd, mchar* file_name, const mchar* file_extension)
{
    Browsefile(hWnd, file_name, file_extension);
    return GetOpenFileName(&ofn);
}

static bool BrowseForSavefile (HWND hWnd, mchar* file_name)
{
    Browsefile(hWnd, file_name, get_extension_from_name(NULL,file_name));
    ofn.Flags |= OFN_OVERWRITEPROMPT;
    return GetSaveFileName(&ofn);
}



// save entry text field in a file
bool save_file_as (const mchar* fileName)
{
    mchar filename [MAX_FILE_NAME];
    HWND hWnd = hWnd_main_window;

    if(fileName==NULL)
    {
        fileName = filename;
        strcpy22 (filename, file_name);
        if(!BrowseForSavefile (hWnd, filename)) return FALSE;
    }

    if(!Savefile (fileName, userinterface_get_text(UI_main_text), -1))
    {
        sprintf2(errormessage, L"Failed to save the file:\r\n%s", fileName);
        MessageBox(hWnd, errormessage, L"Error", MB_OK);
        return FALSE;
    }
    if(file_name != fileName)
    {
        file_exists = true;
        strcpy22 (file_name, fileName);
        SetWindowTitle (hWnd, file_name, file_exists);
        get_extension_from_name (file_extension, file_name);
    }
    if(SendMessage (hWnd_main_text, EM_GETMODIFY, 0, 0))
        set_undo(hWnd_main_text, 1);
    SendMessage (hWnd_main_text, EM_SETMODIFY, FALSE, 0);
    return TRUE;
}

// save entry text field in currently opened file
bool save_file ()
{ return save_file_as (file_exists ? file_name : NULL); }



// open a file and load into entry text field
bool open_file (const mchar* fileName)
{
    size_t i, size;
    mchar* Content = NULL;
    mchar filename [MAX_FILE_NAME];
    HWND hWnd = hWnd_main_window;

    if(!check_save_changes()) return FALSE;

    if(fileName==NULL)
    {
        fileName = filename;
        filename[0] = 0;
        if(!BrowseForOpenfile(hWnd, filename, file_extension)) return FALSE;
    }
    if(!Openfile (fileName, &Content, &size))
    {
        sprintf2(errormessage, L"Failed to open the file:\r\n%s", fileName);
        MessageBox(hWnd, errormessage, L"Error", MB_OK);
        return FALSE;
    }
    for(i=0; i<size; i++) if(Content[i]==0) Content[i] = 0xFFFF;

    if(file_name != fileName)
    {
        file_exists = true;
        strcpy22 (file_name, fileName);
        SetWindowTitle (hWnd, file_name, file_exists);
        get_extension_from_name (file_extension, file_name);
    }
    SendMessage (hWnd_main_text, WM_SETTEXT, 0, (LPARAM)Content);
    set_undo (hWnd_main_text, 0);

    mchar_free(Content);
    return TRUE;
}

// reload the currently opened file
bool reload_file ()
{
    if(!file_exists) return FALSE;
    return open_file (file_name);
}



// start a new and empty file entry
bool new_file ()
{
    HWND hWnd = hWnd_main_window;
    if(!check_save_changes()) return FALSE;

    file_exists = false;
    strcpy21 (file_name, "newfile.txt");
    SetWindowTitle (hWnd, file_name, file_exists);
    file_extension[0]=0;

    SendMessage (hWnd_main_text , WM_SETTEXT, 0, 0);
    set_undo (hWnd_main_text, 0);
    return TRUE;
}



// notify user if entry text field has been modified
bool check_save_changes ()
{
    mchar temp[MAX_FILE_NAME];

    if(!SendMessage (hWnd_main_text, EM_GETMODIFY, 0, 0))
        return TRUE; // no changes to save

    strcpy22(temp, L"Do you want to save changes to ");
    strcat22(temp, get_name_from_path_name(NULL, file_name));
    strcat22(temp, L" ?");

    switch(MessageBox (hWnd_main_window, temp, L"Save changes?", MB_YESNOCANCEL))
    {
    case IDNO: return TRUE;
    case IDYES: if(save_file()) return TRUE;
    default: SetFocus(hWnd_focused); return FALSE;
    }
}



static mchar* set_rodt_of_objects (mchar* str, List* list)
{
    // go through the list of camera objects
    // start from list_tail() so to resolve
    // container-type dependencies
    Object* object = (Object*)list_tail(list);
    for( ; object != NULL; object = (Object*)list_prev(object))
    {
        if(!object->container) continue;
        // set the RODT of each object
        str = strcpy21(str, "\r\n\\mfet{");
        str = strcpy23(str, c_mfet(object->container));
        str = strcpy21(str, "}\r\n");
    }
    return str;
}

bool save_all_objects ()
{
    #ifdef LIBRODT
    mchar fileName[MAX_FILE_NAME];
    mchar *str, *fileContent;
    HWND hWnd = hWnd_main_window;

    if(surface_list->size==0 && camera_list->size==0)
    { MessageBox(hWnd, L"No object to save.", L"Error", MB_OK); return FALSE; }

    fileContent = errormessage;
    str = fileContent;

    // get the file name to save output to
    strcpy21(fileName, "save_all_objects.rodt");
    if(!BrowseForSavefile (hWnd, fileName)) return FALSE;

    // set the file header
    str = strcpy21(str, "0;\r\n\r\ntype = Rhyscitlema_Object_Definition_Text ;\r\n");

    // set the user message
    str = strcpy21(str, "\r\nmessage = \"");
    str = strcpy22(str, userMessage());
    str = strcpy21(str, "\"\r\n");

    str = set_rodt_of_objects (str, camera_list);
    str = set_rodt_of_objects (str, surface_list);

    // now save the output string to file
    if(!Savefile (fileName, fileContent, -1))
    {
        sprintf2(errormessage, L"Failed to save the file:\r\n%s", fileName);
        MessageBox(hWnd, errormessage, L"Error", MB_OK);
        return false;
    }
    #endif
    return true;
}



#include <rwif.h>
void take_camera_picture ()
{
#ifdef LIBRODT
    static bool firsttime = true;
    ImageData imagedata;
    Camera* camera;
    mchar filename[300];
    Mouse* mouse = headMouse;
    HWND hWnd = hWnd_main_window;

    camera = mouse->clickedCamera;
    if(camera==NULL || (Object*)camera != mouse->clickedObject)
    { MessageBox(hWnd, L"First select the camera to take a picture of.", L"Error", MB_OK); return; }

    if(firsttime)
    {
        firsttime = false;
        MessageBox (hWnd, L"The output file format is\r\n"
                          L"specified by the file extension.\r\n"
                          L"\r\nIt can be: .png , .jpg or .bmp\n\r", L"Note", MB_OK);
    }
    strcpy21(filename, "picture.png");
    if(!BrowseForSavefile (hWnd, filename)) return;

    imagedata.pixelArray = (unsigned char*)camera->pixelColour;
    imagedata.height = camera->YSize;
    imagedata.width = camera->XSize;
    imagedata.bpp = 4*8;

    if(write_image_file (CST12(filename), &imagedata))
    {
        sprintf2(errormessage, CST21("Successfully saved %s."), get_name_from_path_name(NULL,filename));
        display_message(errormessage);
    }
    else
    {
        strcpy21 (errormessage, rwif_error_message);
        display_message(errormessage);
        MessageBox(hWnd, errormessage, L"Error", MB_OK);
    }
#endif
}
