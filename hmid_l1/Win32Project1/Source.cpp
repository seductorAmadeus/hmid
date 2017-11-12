#include<windows.h>
#include <time.h>
#include <stdio.h>
#include <iostream> 
#include <fstream>
#include "resource.h"

#define OPEN_ID	1
#define RUN_BITBLT_ID	2
#define RUN_SETPIXEL_ID	3

LPCTSTR getFileName();
int saveBitmap(HDC hdc, HBITMAP bm, int width, int height);
LRESULT CALLBACK handleWindowEvents(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK AboutDlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);
void setPixelDisplay(HWND hwnd, HBITMAP hBitmap);
void setBlt(HWND hWnd, HBITMAP hBitmap);

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
	AppendMenu(MainMenu, MF_STRING, RUN_BITBLT_ID, L"Change");
	AppendMenu(MainMenu, MF_STRING, RUN_SETPIXEL_ID, L"GetPixel");
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
					setBlt(hwnd, hBitmap);
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

void setBlt(HWND hwnd, HBITMAP hBitmap)
{
	BITMAP bm;
	HDC hCompatibleDC = CreateCompatibleDC(NULL);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hCompatibleDC, hBitmap);

	GetObject(hBitmap, sizeof(bm), &bm);

	SelectObject(hCompatibleDC, GetStockObject(DC_BRUSH));
	SetDCBrushColor(hCompatibleDC, RGB(255, 0, 0));

	BitBlt(hCompatibleDC, 0, 0, bm.bmWidth, bm.bmHeight, hCompatibleDC, 0, 0, MERGECOPY);
	saveBitmap(hCompatibleDC, hBitmap, bm.bmWidth, bm.bmHeight);

	InvalidateRect(hwnd, NULL, TRUE);

	SelectObject(hCompatibleDC, hOldBitmap);
	DeleteDC(hCompatibleDC);
}

void setPixelDisplay(HWND hwnd, HBITMAP hBitmap)
{
	BITMAP bm;
	GetObject(hBitmap, sizeof(bm), &bm);

	HDC hCompatibleDC = CreateCompatibleDC(NULL);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hCompatibleDC, hBitmap);


	for (int i = 0; i < bm.bmWidth; i++)
	{
		for (int j = 0; j < bm.bmHeight; j++)
		{
			//COLORREF clr = GetPixel(hCompatibleDC, i, j);
			//SetPixel(hCompatibleDC, i, j, RGB(GetRValue(clr), 0, 0));
			SetPixel(hCompatibleDC, i, j,
					 GetPixel(hCompatibleDC, i, j) | 0x00FF00);
		}
	}

	SelectObject(hCompatibleDC, hOldBitmap);
	saveBitmap(hCompatibleDC, hBitmap, bm.bmWidth, bm.bmHeight);
	InvalidateRect(hwnd, NULL, TRUE);
	DeleteDC(hCompatibleDC);
	DeleteObject(hOldBitmap);
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

	HANDLE hFile = CreateFile(L"C:/Users/admin/Desktop/mmi/Lab1_MNI/Debug/result5551.bmp",
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

	//Unlock and Free the DIB from the heap
	GlobalUnlock(hDIB);
	GlobalFree(hDIB);

	//Close the handle for the file that was created
	CloseHandle(hFile);

	return 0;
}