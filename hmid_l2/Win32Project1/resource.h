//{{NO_DEPENDENCIES}}
// Microsoft Visual C++ generated include file.
// Used by Win32Project1.rc

// Следующие стандартные значения для новых объектов
// 
#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS
#define _APS_NEXT_RESOURCE_VALUE        101
#define _APS_NEXT_COMMAND_VALUE         40001
#define _APS_NEXT_CONTROL_VALUE         1001
#define _APS_NEXT_SYMED_VALUE           101
#endif

#endif
#define IDM_ABOUT 12
#define OPEN_ID	1
#define RUN_MAINTASK_ID 7

typedef struct bmpSize
{
	long width;
	long height;
} bmpSize_t;

LPCTSTR getFileName();
int saveBitmap(HDC hdc, HBITMAP bm, int width, int height);
LRESULT CALLBACK handleWindowEvents(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK AboutDlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);
void runReplaceColorTask(HWND hWnd, HBITMAP hBitmap);
static void changeBMPColors(HDC deviceCtx, HBITMAP bmpImage, BITMAP bmpInfo);

bool if_is_loaded = false;
HBITMAP hBitmap;