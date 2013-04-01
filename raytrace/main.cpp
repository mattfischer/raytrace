#include <windows.h>
#include <commctrl.h>

#include "Trace/Tracer.hpp"
#include "Parse/Parser.hpp"
#include "resource.h"

#define CLASSNAME "raytrace"

HWND hWnd;
HWND hDlg;
HDC backBuffer;

Trace::Tracer *tracer;

void Render()
{
	int x;
	int y;
	int width;
	int height;
	RECT rect;
	Object::Color c;
	HBITMAP backBitmap;
	HGDIOBJ oldBitmap;
	HDC hDC;
	BYTE *bits;
	BITMAPINFO bi;
	MSG msg;

	width = tracer->settings().width;
	height = tracer->settings().height;

	SetWindowPos(hWnd, NULL, 0, 0, width, height, SWP_NOMOVE);
	hDC = GetDC(hWnd);
	backBitmap = CreateCompatibleBitmap(hDC, width, height);
	ReleaseDC(hWnd, hDC);
	oldBitmap = SelectObject(backBuffer, (HGDIOBJ)backBitmap);
	DeleteObject(oldBitmap);

	bits = (BYTE*)malloc(width * 3);
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth = width;
	bi.bmiHeader.biHeight = height;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 24;
	bi.bmiHeader.biCompression = BI_RGB;

	rect.left = 0;
	rect.right = width;

	for(int y=0; y<height; y++)
	{
		for(x=0; x<width; x++)
		{
			Object::Color c = tracer->tracePixel(x, y);

			bits[x*3] = c.blue() * 0xFF;
			bits[x*3 + 1] = c.green() * 0xFF;
			bits[x*3 + 2] = c.red() * 0xFF;
		}

		SetDIBits(backBuffer, backBitmap, height - y - 1, 1, bits, &bi, DIB_RGB_COLORS);
		rect.top = y;
		rect.bottom = y + 1;
		InvalidateRect(hWnd, &rect, FALSE);

		while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
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
			break;

		case WM_LBUTTONUP:
			ShowWindow(hDlg, SW_SHOW);
			break;
	}

	return DefWindowProc(hWnd, iMsg, wParam, lParam);
}

INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		CheckDlgButton(hwndDlg, IDC_LIGHTING, tracer->settings().lighting ? BST_CHECKED : BST_UNCHECKED);
		SetDlgItemInt(hwndDlg, IDC_ANTIALIAS, tracer->settings().antialias, TRUE);
		SendDlgItemMessage(hwndDlg, IDC_ANTIALIAS_SPIN, UDM_SETRANGE, 0, MAKELPARAM(20, 1));
		SetDlgItemInt(hwndDlg, IDC_WIDTH, tracer->settings().width, TRUE);
		SetDlgItemInt(hwndDlg, IDC_HEIGHT, tracer->settings().height, TRUE);
		return FALSE;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case ID_RENDER:
			tracer->settings().lighting = IsDlgButtonChecked(hDlg, IDC_LIGHTING);
			tracer->settings().width = GetDlgItemInt(hDlg, IDC_WIDTH, NULL, TRUE);
			tracer->settings().height = GetDlgItemInt(hDlg, IDC_HEIGHT, NULL, TRUE);
			tracer->settings().antialias = GetDlgItemInt(hDlg, IDC_ANTIALIAS, NULL, TRUE);
			EnableWindow(GetDlgItem(hwndDlg, ID_RENDER), FALSE);
			Render();
			EnableWindow(GetDlgItem(hwndDlg, ID_RENDER), TRUE);
			return TRUE;
		}
		break;

	case WM_CLOSE:
		ShowWindow(hwndDlg, SW_HIDE);
		return TRUE;
	}
	return FALSE;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine, int iCmdShow)
{
	MSG msg;
	WNDCLASSEX wc;
	HDC hDC;

	InitCommonControls();

	AST *ast = Parse::Parser::parse("scene.txt");
	Object::Scene *scene = Object::Scene::fromAST(ast);

	Trace::Tracer::Settings settings;

	settings.width = 800;
	settings.height = 600;
	settings.lighting = true;
	settings.maxRayGeneration = 2;
	settings.antialias = 1;
	settings.hFOV = 45;

	tracer = new Trace::Tracer(scene, settings);

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = LoadIcon(hInst, IDI_APPLICATION);
	wc.hCursor = LoadCursor(hInst, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = CLASSNAME;
	wc.hIconSm = NULL;

	RegisterClassEx(&wc);

	hWnd = CreateWindowEx((DWORD)0, CLASSNAME, "Raytrace", WS_POPUPWINDOW | WS_CAPTION | WS_VISIBLE, 10, 10, settings.width, settings.height, NULL, NULL, hInst, NULL);

	hDC = GetDC(hWnd);
	backBuffer = CreateCompatibleDC(hDC);
	ReleaseDC(hWnd, hDC);

	hDlg = CreateDialog(hInst, (LPCSTR)IDD_RENDER_CONTROL, NULL, DialogProc);
	SetWindowPos(hDlg, NULL, tracer->settings().width + 40, 40, 0, 0, SWP_NOSIZE);
	ShowWindow(hDlg, SW_SHOW);

	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}


