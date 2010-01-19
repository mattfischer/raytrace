#include <windows.h>

#include "trace.h"
#include "buildscene.h"

#define CLASSNAME "raytrace"

HWND hWnd;
HDC backBuffer;
HBITMAP backBitmap;

int screenX = 800;
int screenY = 600;

BOOL PlotNextPixels(const Tracer &tracer)
{
	int x;
	static int y = 0;
	RECT rect;
	Color c;

	BYTE *bits = (BYTE*)malloc(screenX * 3);
	BITMAPINFO bi;

	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth = screenX;
	bi.bmiHeader.biHeight = screenY;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 24;
	bi.bmiHeader.biCompression = BI_RGB;
	
	for(x=0; x<screenX; x++)
	{
		c = tracer.tracePixel(x, y, screenX, screenY);

		bits[x*3] = c.blue() * 0xFF;
		bits[x*3 + 1] = c.green() * 0xFF;
		bits[x*3 + 2] = c.red() * 0xFF;
	}

	SetDIBits(backBuffer, backBitmap, screenY - y - 1, 1, bits, &bi, DIB_RGB_COLORS);
	
	free(bits);

	rect.left = 0;
	rect.right = screenX;
	rect.top = y;
	rect.bottom = y + 1;

	InvalidateRect(hWnd, &rect, FALSE);

	y++;

	return y >= screenY;
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

	Tracer tracer(buildScene(screenX, screenY));

	do
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if(PlotNextPixels(tracer)) break;
	} while(msg.message != WM_QUIT);

	long endTime = GetTickCount();

	char buffer[500];
	sprintf(buffer, "Render took %f seconds\n", (float)(endTime - startTime) / 1000.0);
	OutputDebugString(buffer);

	if(msg.message != WM_QUIT)
	{
		while(GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return 0;
}


