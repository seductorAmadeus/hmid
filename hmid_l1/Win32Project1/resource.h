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
#define RUN_BITBLT_ID	2
#define RUN_SETPIXEL_ID	3
#define RUN_TESTS_ID 4

LPCTSTR getFileName();
int saveBitmap(HDC hdc, HBITMAP bm, int width, int height);
LRESULT CALLBACK handleWindowEvents(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK AboutDlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);
int runBitBltFilter(HWND hWnd, HBITMAP hBitmap);
void runSetPixelFilter(HWND hwnd, HBITMAP hBitmap);
void runTests(HWND hwnd);

bool isLoaded = false;
HBITMAP hBitmap;