#include <windows.h>
#include <commctrl.h>

#include "Parse/Parser.hpp"
#include "RenderEngine.hpp"
#include "Resource.h"

#define CLASSNAME "raytrace"

HWND hWnd;
HWND hDlg;
HDC backBuffer;

Object::Scene *scene;
Trace::Tracer::Settings settings;

#define WM_UPDATE_BITMAP WM_USER

struct CallbackData {
	unsigned char *bits;
	HBITMAP backBitmap;
	BITMAPINFO bi;
	HANDLE evnt;
	DWORD startTime;
};

void lineCallback(int line, void *data)
{
	CallbackData *cbd = (CallbackData*)data;

	PostMessage(hWnd, WM_UPDATE_BITMAP, (WPARAM)cbd, (LPARAM)line);
	WaitForSingleObject(cbd->evnt, INFINITE);

	RECT rect;
	rect.left = 0;
	rect.right = settings.width;
	rect.top = line;
	rect.bottom = line + 1;
	InvalidateRect(hWnd, &rect, FALSE);
}

void doneCallback(RenderEngine *engine, void *data)
{
	CallbackData *cbd = (CallbackData*)data;

	DWORD endTime = GetTickCount();
	char buf[256];
	sprintf(buf, "Render time: %ims", endTime - cbd->startTime);
	SetDlgItemText(hDlg, IDC_RENDER_TIME, buf);

	CloseHandle(cbd->evnt);
	free(cbd->bits);
	delete(cbd);
	delete engine;
}

void Render()
{
	int width;
	int height;
	HGDIOBJ oldBitmap;
	HDC hDC;
	HBITMAP backBitmap;

	width = settings.width;
	height = settings.height;

	SetWindowPos(hWnd, NULL, 0, 0, width, height, SWP_NOMOVE);
	hDC = GetDC(hWnd);
	backBitmap = CreateCompatibleBitmap(hDC, width, height);
	ReleaseDC(hWnd, hDC);
	oldBitmap = SelectObject(backBuffer, (HGDIOBJ)backBitmap);
	DeleteObject(oldBitmap);

	CallbackData *cbd = new CallbackData;
	cbd->bits = (BYTE*)malloc(width * 3);
	cbd->bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	cbd->bi.bmiHeader.biWidth = width;
	cbd->bi.bmiHeader.biHeight = height;
	cbd->bi.bmiHeader.biPlanes = 1;
	cbd->bi.bmiHeader.biBitCount = 24;
	cbd->bi.bmiHeader.biCompression = BI_RGB;
	cbd->backBitmap = backBitmap;
	cbd->evnt = CreateEvent(NULL, FALSE, FALSE, NULL);

	cbd->startTime = GetTickCount();
	RenderEngine *engine = new RenderEngine(scene, settings);
	engine->render(cbd->bits, lineCallback, doneCallback, cbd);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	int x, y;
	CallbackData *cbd;

	switch(iMsg)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_UPDATE_BITMAP:
			cbd = (CallbackData*)wParam;
			SetDIBits(backBuffer, cbd->backBitmap, settings.height - lParam - 1, 1, cbd->bits, &cbd->bi, DIB_RGB_COLORS);
			SetEvent(cbd->evnt);
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
		CheckDlgButton(hwndDlg, IDC_LIGHTING, settings.lighting ? BST_CHECKED : BST_UNCHECKED);
		SetDlgItemInt(hwndDlg, IDC_ANTIALIAS, settings.antialias, TRUE);
		SendDlgItemMessage(hwndDlg, IDC_ANTIALIAS_SPIN, UDM_SETRANGE, 0, MAKELPARAM(20, 1));
		SetDlgItemInt(hwndDlg, IDC_WIDTH, settings.width, TRUE);
		SetDlgItemInt(hwndDlg, IDC_HEIGHT, settings.height, TRUE);
		return FALSE;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case ID_RENDER:
			settings.lighting = IsDlgButtonChecked(hDlg, IDC_LIGHTING);
			settings.width = GetDlgItemInt(hDlg, IDC_WIDTH, NULL, TRUE);
			settings.height = GetDlgItemInt(hDlg, IDC_HEIGHT, NULL, TRUE);
			settings.antialias = GetDlgItemInt(hDlg, IDC_ANTIALIAS, NULL, TRUE);
			EnableWindow(GetDlgItem(hwndDlg, ID_RENDER), FALSE);
			SetDlgItemText(hDlg, IDC_RENDER_TIME, "");
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
	scene = Object::Scene::fromAST(ast);

	settings.width = 800;
	settings.height = 600;
	settings.lighting = true;
	settings.maxRayGeneration = 2;
	settings.antialias = 1;

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
	SetWindowPos(hDlg, NULL, settings.width + 40, 40, 0, 0, SWP_NOSIZE);
	ShowWindow(hDlg, SW_SHOW);

	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}


