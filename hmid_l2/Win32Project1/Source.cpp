#include <windows.h>
#include <time.h>
#include <stdio.h>
#include <iostream> 
#include <fstream>
#include "resource.h"
#include <Commdlg.h>
#include <io.h>
#include <fcntl.h>
#include <winuser.h>
#include <debugapi.h>

#define OPEN_ID	1
#define RUN_BITBLT_ID	2
#define RUN_SETPIXEL_ID	3
#define RUN_TESTS_ID 4
#define RUN_MAINTASK_ID 7
#define COLORREF2RGB(Color) (Color & 0xff00) | ((Color >> 16) & 0xff) \
                                 | ((Color << 16) & 0xff0000)
HBITMAP BlitReplaceColor(HBITMAP hBmp, COLORREF cOldColor, COLORREF cNewColor, HDC hBmpDC);

LPCTSTR getFileName();
int saveBitmap(HDC hdc, HBITMAP bm, int width, int height);
LRESULT CALLBACK handleWindowEvents(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK AboutDlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);
int runBitBltFilter(HWND hWnd, HBITMAP hBitmap);
void runSetPixelFilter(HWND hwnd, HBITMAP hBitmap);
void runTests(HWND hwnd); 
void applyDoubleBlueChannel(HWND hwnd, HBITMAP hBitmap);
HBITMAP ReplaceColor(HBITMAP hBmp, COLORREF cOldColor, COLORREF cNewColor, HDC hBmpDC);
void runReplaceColorTask(HWND hWnd, HBITMAP hBitmap);
void transformBitmap(HWND hWnd, HBITMAP hBitmap);



bool isLoaded = false;
HBITMAP hBitmap;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	WNDCLASS WndClass;
	MSG Msg;
	wchar_t szClassName[] = L"Лабораторная работа №1";

	// Set window attributes
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = handleWindowEvents;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszMenuName = L"Menu";
	WndClass.lpszClassName = szClassName;

	// Register window class
	if (!RegisterClass(&WndClass))
	{
		MessageBox(NULL, L"Cannot register class", L"Error", MB_OK);
		return 0;
	}

	// Create window
	hWnd = CreateWindow(szClassName, L"Лабораторная работа №1",
						WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
						CW_USEDEFAULT, CW_USEDEFAULT,
						CW_USEDEFAULT, NULL, NULL,
						hInstance, NULL);

	HMENU MainMenu = CreateMenu();
	AppendMenu(MainMenu, MF_STRING, OPEN_ID, L"Open");
	AppendMenu(MainMenu, MF_STRING, RUN_MAINTASK_ID, L"Apply");
	AppendMenu(MainMenu, MF_STRING, RUN_BITBLT_ID, L"Change");
	AppendMenu(MainMenu, MF_STRING, RUN_SETPIXEL_ID, L"SetPixel");
	AppendMenu(MainMenu, MF_STRING, RUN_TESTS_ID, L"Tests");
	AppendMenu(MainMenu, MF_STRING, IDM_ABOUT, L"About");

	SetMenu(hWnd, MainMenu);

	if (!hWnd)
	{
		MessageBox(NULL, L"Cannot create window", L"Error", MB_OK);
		return 0;
	}

	// Show window, start handling his messages
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&Msg, NULL, 0, 0))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	return Msg.wParam;
}

LRESULT CALLBACK handleWindowEvents(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HINSTANCE hInstance;

	switch (message)
	{

		case WM_CREATE:
			{
				hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
				return 0;
			}

		case WM_DESTROY:
			{
				PostQuitMessage(0);
				break;
			}

		case WM_PAINT:
			{
				BITMAP bm;
				PAINTSTRUCT ps;
				HDC hDC = BeginPaint(hwnd, &ps);

				HDC hCompatibleDC = CreateCompatibleDC(hDC);
				HBITMAP hOldBitmap = (HBITMAP)SelectObject(hCompatibleDC, hBitmap);
				GetObject(hBitmap, sizeof(bm), &bm);
				BitBlt(hDC, 0, 0, bm.bmWidth, bm.bmHeight, hCompatibleDC, 0, 0, SRCCOPY);
				SelectObject(hCompatibleDC, hOldBitmap);
				DeleteDC(hCompatibleDC);

				EndPaint(hwnd, &ps);
				break;
			}

		case WM_COMMAND:
			{

				if (LOWORD(wParam) == IDM_ABOUT)
				{
					DialogBox(hInstance, L"AboutBox", hwnd, AboutDlgProc);
					return 0;
				}

				if (LOWORD(wParam) == OPEN_ID)
				{
					LPCTSTR fileName = getFileName();
					hBitmap = (HBITMAP)LoadImage(0, fileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
					InvalidateRect(hwnd, NULL, TRUE);
					isLoaded = true;
				}

				if (LOWORD(wParam) == RUN_BITBLT_ID)
				{
					if (!isLoaded)
					{
						break;
					}
					if (runBitBltFilter(hwnd, hBitmap) == 1) // if error
					{
						break;
					}
				}

				if (LOWORD(wParam) == RUN_MAINTASK_ID)
				{
					if (!isLoaded)
					{
						break;
					}
					
					runReplaceColorTask(hwnd, hBitmap);
				}

				if (LOWORD(wParam) == RUN_SETPIXEL_ID)
				{
					if (!isLoaded)
					{
						break;
					}
					runSetPixelFilter(hwnd, hBitmap);
				}

				if (LOWORD(wParam) == RUN_TESTS_ID)
				{
					runTests(hwnd);
				}

				break;
			}
		default:
			{
				return DefWindowProc(hwnd, message, wParam, lParam);
			}
	}
	return 0;
}

BOOL CALLBACK AboutDlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
		case WM_INITDIALOG:
			return TRUE;
		case WM_PAINT:
			{
				BITMAP bm;
				PAINTSTRUCT ps;
				HDC hDC = BeginPaint(hDlg, &ps);

				HDC hCompatibleDC = CreateCompatibleDC(hDC);
				HBITMAP hOldBitmap = (HBITMAP)SelectObject(hCompatibleDC, hBitmap);
				GetObject(hBitmap, sizeof(bm), &bm);
				BitBlt(hDC, 0, 0, bm.bmWidth, bm.bmHeight, hCompatibleDC, 0, 0, SRCCOPY);
				SelectObject(hCompatibleDC, hOldBitmap);
				DeleteDC(hCompatibleDC);

				EndPaint(hDlg, &ps);
				break;
			}
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDOK:
				case IDCANCEL:
					EndDialog(hDlg, 0);
					return TRUE;
			}
			break;
	}
	return FALSE;
}

void runReplaceColorTask(HWND hWnd, HBITMAP hBitmap)
{
	BITMAP bitmap;
	HDC hCompatibleDC = CreateCompatibleDC(NULL); /* Create Device_Context compatible with current window */
	SelectObject(hCompatibleDC, hBitmap); /* Select hBitmap in hCompatibleDC context */
	GetObject(hBitmap, sizeof(bitmap), &bitmap); /* Get BITMAP size */

	HBITMAP hBmp = BlitReplaceColor(hBitmap, 0xFF0000, 0x00ff00, hCompatibleDC); // replace blue by green
	
	saveBitmap(hCompatibleDC, hBmp, bitmap.bmWidth, bitmap.bmHeight);
	InvalidateRect(hWnd, NULL, TRUE); /* Set rectangle for redraw window */
	DeleteObject(hBmp);

	DeleteDC(hCompatibleDC); /* Delete compatible contex */
}

int runBitBltFilter(HWND hWnd, HBITMAP hBitmap)
{

	BITMAP bitmap;
	HDC hCompatibleDC = CreateCompatibleDC(NULL); /* Create Device_Context compatible with current window */
	SelectObject(hCompatibleDC, hBitmap); /* Select hBitmap in hCompatibleDC context */
	GetObject(hBitmap, sizeof(bitmap), &bitmap); /* Get BITMAP size */
	SelectObject(hCompatibleDC, GetStockObject(DC_BRUSH)); /* Select DC_BRUSH in hCompatibleDC context */
	SetDCBrushColor(hCompatibleDC, RGB(255, 0, 255)); /* Set in current brush context color(255,0,255) */
	if (!BitBlt(hCompatibleDC, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hCompatibleDC, 0, 0, MERGECOPY))
	{
		return 1;
	}
	saveBitmap(hCompatibleDC, hBitmap, bitmap.bmWidth, bitmap.bmHeight);
	InvalidateRect(hWnd, NULL, TRUE); /* Set rectangle for redraw window */
	DeleteDC(hCompatibleDC); /* Delete compatible contex */
	return 0;
}

void runSetPixelFilter(HWND hwnd, HBITMAP hBitmap)
{

	BITMAP bitmap;
	GetObject(hBitmap, sizeof(bitmap), &bitmap);
	HDC hCompatibleDC = CreateCompatibleDC(NULL);
	SelectObject(hCompatibleDC, hBitmap);
	for (int i = 0; i < bitmap.bmWidth; i++)
	{
		for (int j = 0; j < bitmap.bmHeight; j++)
		{
			SetPixel(hCompatibleDC, i, j, GetPixel(hCompatibleDC, i, j) & 0xFF00FF);
		}
	}
	saveBitmap(hCompatibleDC, hBitmap, bitmap.bmWidth, bitmap.bmHeight);
	InvalidateRect(hwnd, NULL, TRUE);
	DeleteDC(hCompatibleDC);
}
HBITMAP BlitReplaceColor(HBITMAP hBmp, COLORREF cOldColor, COLORREF cNewColor, HDC hBmpDC)
{
	HBITMAP RetBmp = NULL;
	if (hBmp)
	{
		HDC BufferDC = CreateCompatibleDC(NULL);	// DC for Source Bitmap
		if (BufferDC)
		{
			HBITMAP hTmpBitmap = (HBITMAP)NULL;
			if (hBmpDC)
				if (hBmp == (HBITMAP)GetCurrentObject(hBmpDC, OBJ_BITMAP))
				{
					hTmpBitmap = CreateBitmap(1, 1, 1, 1, NULL);
					SelectObject(hBmpDC, hTmpBitmap);
				}
			HGDIOBJ PreviousBufferObject = SelectObject(BufferDC, hBmp);
			// here BufferDC contains the bitmap

			HDC DirectDC = CreateCompatibleDC(NULL);	// DC for working		
			if (DirectDC)
			{
				HDC MaskDC = CreateCompatibleDC(NULL);	// DC for mask
				if (MaskDC)
				{
					// Get bitmap size
					BITMAP bm;
					GetObject(hBmp, sizeof(bm), &bm);

					// create a BITMAPINFO with minimal initilisation for the CreateDIBSection
					BITMAPINFO RGB32BitsBITMAPINFO;
					ZeroMemory(&RGB32BitsBITMAPINFO, sizeof(BITMAPINFO));
					RGB32BitsBITMAPINFO.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
					RGB32BitsBITMAPINFO.bmiHeader.biWidth = bm.bmWidth;
					RGB32BitsBITMAPINFO.bmiHeader.biHeight = bm.bmHeight;
					RGB32BitsBITMAPINFO.bmiHeader.biPlanes = 1;
					RGB32BitsBITMAPINFO.bmiHeader.biBitCount = 32;
					RECT tmprect = { 0,0,bm.bmWidth,bm.bmHeight };

					HBITMAP bitmapTrans = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);
					DeleteObject(SelectObject(MaskDC, bitmapTrans));

					SetBkColor(BufferDC, cOldColor);
					RetBmp = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 32, NULL);
					HGDIOBJ PreviousObject = SelectObject(DirectDC, RetBmp);
					HBRUSH sb = CreateSolidBrush(cNewColor);
					FillRect(DirectDC, &tmprect, sb);
					DeleteObject(sb);

					BitBlt(MaskDC, 0, 0, bm.bmWidth, bm.bmHeight, BufferDC, 0, 0, SRCCOPY);
					BitBlt(DirectDC, 0, 0, bm.bmWidth, bm.bmHeight, BufferDC, 0, 0, SRCINVERT);
					BitBlt(DirectDC, 0, 0, bm.bmWidth, bm.bmHeight, MaskDC, 0, 0, SRCAND);
					BitBlt(DirectDC, 0, 0, bm.bmWidth, bm.bmHeight, BufferDC, 0, 0, SRCINVERT);

					DeleteObject(PreviousObject);
					DeleteObject(bitmapTrans);
					DeleteDC(MaskDC);
				}
				// clean up
				DeleteDC(DirectDC);
			}
			// BufferDC is now useless
			if (hTmpBitmap)
			{
				SelectObject(hBmpDC, hBmp);
				DeleteObject(hTmpBitmap);
			}
			SelectObject(BufferDC, PreviousBufferObject);
			DeleteDC(BufferDC);
		}
	}
	return RetBmp;
}
HBITMAP ReplaceColor(HBITMAP hBmp, COLORREF cOldColor, COLORREF cNewColor, HDC hBmpDC)
{
	HBITMAP RetBmp = NULL;
	if (hBmp)
	{
		HDC BufferDC = CreateCompatibleDC(NULL);    // DC for Source Bitmap
		if (BufferDC)
		{
			HBITMAP hTmpBitmap = (HBITMAP)NULL;
			if (hBmpDC)
				if (hBmp == (HBITMAP)GetCurrentObject(hBmpDC, OBJ_BITMAP))
				{
					hTmpBitmap = CreateBitmap(1, 1, 1, 1, NULL);
					SelectObject(hBmpDC, hTmpBitmap);
				}

			HGDIOBJ PreviousBufferObject = SelectObject(BufferDC, hBmp);
			// here BufferDC contains the bitmap

			HDC DirectDC = CreateCompatibleDC(NULL); // DC for working
			if (DirectDC)
			{
				// Get bitmap size
				BITMAP bm;
				GetObject(hBmp, sizeof(bm), &bm);

				WORD cClrBits = (WORD)(bm.bmPlanes * bm.bmBitsPixel);
				DWORD biSizeImage = ((bm.bmWidth * cClrBits + 31) & ~31) / 8 * bm.bmHeight;

				// create a BITMAPINFO with minimal initilisation 
				// for the CreateDIBSection
				BITMAPINFO RGB32BitsBITMAPINFO;
				ZeroMemory(&RGB32BitsBITMAPINFO, sizeof(BITMAPINFO));
				RGB32BitsBITMAPINFO.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				RGB32BitsBITMAPINFO.bmiHeader.biWidth = bm.bmWidth;
				RGB32BitsBITMAPINFO.bmiHeader.biHeight = bm.bmHeight;
				RGB32BitsBITMAPINFO.bmiHeader.biPlanes = 1;
				RGB32BitsBITMAPINFO.bmiHeader.biBitCount = 32;

				// pointer used for direct Bitmap pixels access
				ULONGLONG * ptPixels;

				HBITMAP DirectBitmap = CreateDIBSection(DirectDC,
					(BITMAPINFO *)&RGB32BitsBITMAPINFO,
														DIB_RGB_COLORS,
														(void **)&ptPixels,
														NULL, 0);
				if (DirectBitmap)
				{
					// here DirectBitmap!=NULL so ptPixels!=NULL no need to test
					HGDIOBJ PreviousObject = SelectObject(DirectDC, DirectBitmap);
					BitBlt(DirectDC, 0, 0,
						   bm.bmWidth, bm.bmHeight,
						   BufferDC, 0, 0, SRCCOPY);

					// here the DirectDC contains the bitmap

					// Convert COLORREF to RGB (Invert RED and BLUE)
					cOldColor = COLORREF2RGB(cOldColor);
					cNewColor = COLORREF2RGB(cNewColor);

					// After all the inits we can do the job : Replace Color
					for (long long i = ((bm.bmWidth*bm.bmHeight) - 1); i >= 0; i--)
					{
						if (ptPixels[i] == cOldColor) ptPixels[i] = cNewColor;
					}
					// little clean up
					// Don't delete the result of SelectObject because it's 
					// our modified bitmap (DirectBitmap)
					SelectObject(DirectDC, PreviousObject);

					// finish
					RetBmp = DirectBitmap;
				}
				// clean up
				DeleteDC(DirectDC);
			}
			if (hTmpBitmap)
			{
				SelectObject(hBmpDC, hBmp);
				DeleteObject(hTmpBitmap);
			}
			SelectObject(BufferDC, PreviousBufferObject);
			// BufferDC is now useless
			DeleteDC(BufferDC);
		}
	}
	return RetBmp;
}

void transformBitmap(HWND hWnd, HBITMAP hBitmap)
{
	BITMAP bm;
	HDC hdc;
	GetObject(hBitmap, sizeof(BITMAP), &bm);

	BITMAPINFOHEADER   bi;

	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bm.bmWidth;
	bi.biHeight = bm.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	DWORD dwBmpSize = ((bm.bmWidth * bi.biBitCount + 31) / 32) * 4 * bm.bmHeight;

	HANDLE hDIB = GlobalAlloc(GHND, dwBmpSize);
	unsigned char *lpbitmapBlue  = (unsigned char *)GlobalLock(hDIB);
	unsigned char *lpbitmapGreen = (unsigned char *)GlobalLock(hDIB);
	unsigned char *lpbitmapRed = (unsigned char *)GlobalLock(hDIB);

	hdc = GetDC(hWnd);
	GetDIBits(hdc, hBitmap, 0,
		(UINT)bm.bmHeight,
			  lpbitmapBlue ,
			  (BITMAPINFO *)&bi, DIB_RGB_COLORS);
	GetDIBits(hdc, hBitmap, 0,
		(UINT)bm.bmHeight,
			  lpbitmapGreen,
			  (BITMAPINFO *)&bi, DIB_RGB_COLORS);
	GetDIBits(hdc, hBitmap, 0,
		(UINT)bm.bmHeight,
			  lpbitmapRed,
			  (BITMAPINFO *)&bi, DIB_RGB_COLORS);

	auto oldPointer = lpbitmapBlue ;
	auto oldPointer2 = lpbitmapGreen;
	//lpbitmap += (0 - access to blue, 1 - to green, 2 - to red)
	lpbitmapBlue  += 0; //(0 - access to blue, 1 - to green, 2 - to red)
	lpbitmapGreen += 2; // access to green
	lpbitmapRed += 2;

	for (DWORD i = 0; i<dwBmpSize; i += 4)
	{
		// изменяем синий на зеленый
		*lpbitmapGreen = *lpbitmapBlue;
		
		lpbitmapBlue  += 4;
		lpbitmapGreen += 4;
	}

	SetDIBits(hdc, hBitmap, 0,
		(UINT)bm.bmHeight,
			  oldPointer,
			  (BITMAPINFO *)&bi, DIB_RGB_COLORS);

	saveBitmap(hdc, hBitmap, bm.bmWidth, bm.bmHeight);

	GlobalUnlock(hDIB);
	GlobalFree(hDIB);
	ReleaseDC(hWnd, hdc);
	InvalidateRect(hWnd, NULL, TRUE);
}

void applyDoubleBlueChannel(HWND hwnd, HBITMAP hBitmap)
{
	BITMAP bmp;
	GetObject(hBitmap, sizeof(BITMAP), &bmp);
	HDC hDCMem = CreateCompatibleDC(NULL);
	HGDIOBJ oldBitmap = SelectObject(hDCMem, hBitmap);
	WORD cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);
	DWORD biSizeImage = ((bmp.bmWidth * cClrBits + 31) & ~31) / 8 * bmp.bmHeight;
	BITMAPINFO bmi = { 0 };

	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = bmp.bmWidth;
	bmi.bmiHeader.biHeight = bmp.bmHeight;
	bmi.bmiHeader.biPlanes = bmp.bmPlanes;
	bmi.bmiHeader.biBitCount = bmp.bmBitsPixel;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = biSizeImage;
	bmi.bmiHeader.biClrImportant = 0;

	LPBYTE lpBits = (LPBYTE)GlobalAlloc(GMEM_FIXED, biSizeImage); // memory pointer
	GetDIBits(hDCMem, hBitmap, 0, bmp.bmHeight, lpBits, &bmi, DIB_RGB_COLORS);
	WORD buf;

	COLORREF cOldColor = COLORREF2RGB(0x0000FF);
	COLORREF cNewColor = COLORREF2RGB(0x00ff00);

	for (long i = 0; i < bmp.bmWidth * bmp.bmHeight * 3; i += 3)
	{
		if (lpBits[i] == cOldColor)
		{
			lpBits[i] = cNewColor;
		}
	}

	SetDIBits(hDCMem, hBitmap, 0, bmp.bmHeight, lpBits, &bmi, DIB_RGB_COLORS);
	saveBitmap(hDCMem, hBitmap, bmp.bmWidth, bmp.bmHeight);

	SelectObject(hDCMem, oldBitmap);

	GlobalFree((HGLOBAL)lpBits);
	DeleteDC(hDCMem);
	DeleteObject(oldBitmap);
	InvalidateRect(hwnd, NULL, TRUE);
}

LPCTSTR getFileName()
{
	OPENFILENAME ofn;

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = new WCHAR[1000];
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = 1000;
	ofn.lpstrFilter = L"BMP\0*.bmp\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	GetOpenFileName(&ofn);

	return ofn.lpstrFile;
}

int saveBitmap(HDC hdc, HBITMAP H, int width, int height)
{
	BITMAPFILEHEADER   bmfHeader;
	BITMAPINFOHEADER   bi;

	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = width;
	bi.biHeight = height;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	DWORD dwBmpSize = ((width * bi.biBitCount + 31) / 32) * 4 * height;

	HANDLE hDIB = GlobalAlloc(GHND, dwBmpSize);
	char *lpbitmap = (char *)GlobalLock(hDIB);

	GetDIBits(hdc, H, 0,
		(UINT)height,
			  lpbitmap,
			  (BITMAPINFO *)&bi, DIB_RGB_COLORS);

	HANDLE hFile = CreateFile(L"C:/Users/admin/Desktop/tphoto/result.bmp",
							  GENERIC_WRITE,
							  0,
							  NULL,
							  CREATE_ALWAYS,
							  FILE_ATTRIBUTE_NORMAL, NULL);

	DWORD dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);
	bmfHeader.bfSize = dwSizeofDIB;
	bmfHeader.bfType = 0x4D42; //BM   

	DWORD dwBytesWritten = 0;
	WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);

	GlobalUnlock(hDIB);
	GlobalFree(hDIB);

	CloseHandle(hFile);

	return 0;
}

void runTests(HWND hwnd)
{
	HBITMAP cpy;

	DWORD dwStartTime;
	DWORD dwElapsed;

	// open console
	AllocConsole();
	HANDLE stdHandle;
	int hConsole;
	FILE* fp;
	stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	hConsole = _open_osfhandle((long)stdHandle, _O_TEXT);
	fp = _fdopen(hConsole, "w");
	freopen_s(&fp, "CONOUT$", "w", stdout);

	// measure BitBlt
	printf("BitBlt: \n");
	for (int i = 1; i <= 10; i++)
	{
		cpy = (HBITMAP)CopyImage(hBitmap, IMAGE_BITMAP, i * 200, i * 200, 0);


		dwStartTime = GetTickCount();

		runBitBltFilter(hwnd, cpy);

		dwElapsed = GetTickCount() - dwStartTime;
		printf("[%4d x %4d] = %d.%d sec.\n", (i * 200), (i * 200), dwElapsed / 1000, dwElapsed - dwElapsed / 1000);

		DeleteObject(cpy);
	}
	printf("\n");


	// measure SetPixel/GetPixel
	printf("Pixel: \n");
	for (int i = 1; i <= 10; i++)
	{
		cpy = (HBITMAP)CopyImage(hBitmap, IMAGE_BITMAP, i * 200, i * 200, 0);


		dwStartTime = GetTickCount();

		runSetPixelFilter(hwnd, cpy);

		dwElapsed = GetTickCount() - dwStartTime;
		printf("[%4d x %4d] = %d.%d sec.\n", (i * 200), (i * 200), dwElapsed / 1000, dwElapsed - dwElapsed / 1000);

		DeleteObject(cpy);
	}
	printf("\nDone!");
}