/**
	Source.cpp
	Purpose: Changes bmp file

	@author Rayla Martin
	@version 1.0 15.10.2017
*/

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

/**
	This method is used as application entry point

	@param hInstance is something called a "handle to an instance".
	@param hPrevInstance has no meaning. It was used in 16-bit Windows, but is now always zero.
	@param lpszCmdParam contains the command-line arguments as a Unicode string.
	@param nCmdShow  is a flag that says whether the main application window will be minimized, maximized, or shown normally.
	@return an int value; I can use it to convey a status code to some other program that i write.
*/
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


/**
	This method is used for handle messages in the queue

	@param hInstance is something called a "handle to an instance".
	@param hPrevInstance has no meaning. It was used in 16-bit Windows, but is now always zero.
	@param lpszCmdParam contains the command-line arguments as a Unicode string.
	@param nCmdShow  is a flag that says whether the main application window will be minimized, maximized, or shown normally.
	@return an int value; I can use it to convey a status code to some other program that i write.
*/
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

/// This method is used for display information about author.
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

/// This method is used for increase the intensity of red and blue colors.
/// For this, the BitBlt function is used
int runBitBltFilter(HWND hWnd, HBITMAP hBitmap)
{

	BITMAP bitmap;
	HDC hCompatibleDC = CreateCompatibleDC(NULL);
	SelectObject(hCompatibleDC, hBitmap);
	GetObject(hBitmap, sizeof(bitmap), &bitmap);
	SelectObject(hCompatibleDC, GetStockObject(DC_BRUSH));
	SetDCBrushColor(hCompatibleDC, RGB(255, 0, 255));
	if (!BitBlt(hCompatibleDC, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hCompatibleDC, 0, 0, MERGECOPY))
	{
		return 1;
	}
	saveBitmap(hCompatibleDC, hBitmap, bitmap.bmWidth, bitmap.bmHeight);
	InvalidateRect(hWnd, NULL, TRUE);
	DeleteDC(hCompatibleDC);
	return 0;
}

/// This method is used for increase the intensity of red and blue colors.
/// For this, the GetPixel and SetPixel function's are used.
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

/// This method just return a file name.
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

/// This method just save new bmp file.
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

/// This method just run tests.
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