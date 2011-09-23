#include <windows.h>

#include "trace.h"
#include "buildscene.h"
#include "resource.h"

#define CLASSNAME "raytrace"

HWND hWnd;
HDC backBuffer;
HBITMAP backBitmap;
HWND hDlg;

int screenX = 800;
int screenY = 600;

Tracer *tracer;

void Render()
{
	int x;
	int y;
	int row;
	RECT rect;
	Color c;

	BYTE *bits = (BYTE*)malloc(screenX * 3);
	BITMAPINFO bi;
	MSG msg;

	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth = screenX;
	bi.bmiHeader.biHeight = screenY;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 24;
	bi.bmiHeader.biCompression = BI_RGB;

	rect.left = 0;
	rect.right = screenX;

	tracer->settings().lighting = IsDlgButtonChecked(hDlg, IDC_LIGHTING);
	row = 0;
	for(int y=0; y<screenY; y++)
	{
		for(x=0; x<screenX; x++)
		{
			Color c = tracer->tracePixel(x, y, screenX, screenY);

			bits[x*3] = c.blue() * 0xFF;
			bits[x*3 + 1] = c.green() * 0xFF;
			bits[x*3 + 2] = c.red() * 0xFF;
		}

		SetDIBits(backBuffer, backBitmap, screenY - y - 1, 1, bits, &bi, DIB_RGB_COLORS);
		rect.top = y;
		rect.bottom = y + 1;
		InvalidateRect(hWnd, &rect, FALSE);

		row++;
		if(row == 1)
		{
			while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			row = 0;
		}
	}
	free(bits);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	int x, y;

	switch(iMsg)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_PAINT:
			BeginPaint(hWnd, &ps);

			BitBlt(ps.hdc, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top, backBuffer, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);

			EndPaint(hWnd, &ps);
	}

	return DefWindowProc(hWnd, iMsg, wParam, lParam);
}

INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		CheckDlgButton(hwndDlg, IDC_LIGHTING, tracer->settings().lighting ? BST_CHECKED : BST_UNCHECKED);
		return FALSE;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case ID_RENDER:
			EnableWindow(GetDlgItem(hwndDlg, ID_RENDER), FALSE);
			Render();
			EnableWindow(GetDlgItem(hwndDlg, ID_RENDER), TRUE);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine, int iCmdShow)
{
	MSG msg;
	WNDCLASSEX wc;
	HDC hDC;

	long startTime = GetTickCount();

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = LoadIcon(hInst, IDI_APPLICATION);
	wc.hCursor = LoadCursor(hInst, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = CLASSNAME;
	wc.hIconSm = NULL;

	RegisterClassEx(&wc);

	hWnd = CreateWindowEx((DWORD)0, CLASSNAME, "Raytrace", WS_POPUPWINDOW | WS_CAPTION | WS_VISIBLE, 10, 10, screenX, screenY, NULL, NULL, hInst, NULL);

	hDC = GetDC(hWnd);
	backBuffer = CreateCompatibleDC(hDC);
	backBitmap = CreateCompatibleBitmap(hDC, screenX, screenY);
	SelectObject(backBuffer, (HGDIOBJ)backBitmap);
	ReleaseDC(hWnd, hDC);

	Scene *scene = buildScene();
	tracer = new Tracer(scene);

	hDlg = CreateDialog(hInst, (LPCSTR)IDD_RENDER_CONTROL, NULL, DialogProc);
	SetWindowPos(hDlg, NULL, screenX + 40, 40, 0, 0, SWP_NOSIZE);
	ShowWindow(hDlg, SW_SHOW);

	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}


