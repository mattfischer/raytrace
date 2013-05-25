#include <windows.h>
#include <commctrl.h>

#include "Parse/Parser.hpp"
#include "RenderEngine.hpp"
#include "Resource.h"
#include "RenderControlDlg.hpp"

#define CLASSNAME "raytrace"

HWND hWnd;
HDC backBuffer;

RenderControlDlg renderControl;

Object::Scene *scene;

struct RenderState : public RenderEngine::Listener {
	unsigned char *bits;
	HBITMAP backBitmap;
	BITMAPINFO bi;
	HANDLE evnt;
	DWORD startTime;
	RenderEngine *engine;

	void onRenderDone();
};

void RenderState::onRenderDone()
{
	DWORD endTime = GetTickCount();
	renderControl.setRenderTime(endTime - startTime);
	KillTimer(hWnd, 0);

	free(bits);
	delete engine;

	delete this;
}

RenderState *renderState;

void Render()
{
	int width;
	int height;
	HGDIOBJ oldBitmap;
	HDC hDC;
	HBITMAP backBitmap;

	width = renderControl.settings().width;
	height = renderControl.settings().height;

	SetWindowPos(hWnd, NULL, 0, 0, width, height, SWP_NOMOVE);
	hDC = GetDC(hWnd);
	backBitmap = CreateCompatibleBitmap(hDC, width, height);
	ReleaseDC(hWnd, hDC);
	oldBitmap = SelectObject(backBuffer, (HGDIOBJ)backBitmap);
	DeleteObject(oldBitmap);

	renderState = new RenderState;
	renderState->bits = (BYTE*)malloc(width * height * 3);
	memset(renderState->bits, 0, width * height * 3);
	renderState->bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	renderState->bi.bmiHeader.biWidth = width;
	renderState->bi.bmiHeader.biHeight = height;
	renderState->bi.bmiHeader.biPlanes = 1;
	renderState->bi.bmiHeader.biBitCount = 24;
	renderState->bi.bmiHeader.biCompression = BI_RGB;
	renderState->backBitmap = backBitmap;

	renderState->startTime = GetTickCount();
	renderState->engine = new RenderEngine(scene, renderControl.settings());
	renderState->engine->render(renderState->bits, renderState);

	SetTimer(hWnd, 0, 0, NULL);
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

		case WM_TIMER:
			SetDIBits(backBuffer, renderState->backBitmap, 0, renderControl.settings().height, renderState->bits, &renderState->bi, DIB_RGB_COLORS);
			InvalidateRect(hWnd, NULL, FALSE);
			SetTimer(hWnd, 0, 0, NULL);
			break;

		case WM_PAINT:
			BeginPaint(hWnd, &ps);

			BitBlt(ps.hdc, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top, backBuffer, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);

			EndPaint(hWnd, &ps);
			break;

		case WM_LBUTTONUP:
			renderControl.show();
			break;
	}

	return DefWindowProc(hWnd, iMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine, int iCmdShow)
{
	MSG msg;
	WNDCLASSEX wc;
	HDC hDC;

	InitCommonControls();

	AST *ast = Parse::Parser::parse("scene.txt");
	scene = Object::Scene::fromAST(ast);

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

	hWnd = CreateWindowEx((DWORD)0, CLASSNAME, "Raytrace", WS_POPUPWINDOW | WS_CAPTION | WS_VISIBLE, 10, 10, renderControl.settings().width, renderControl.settings().height, NULL, NULL, hInst, NULL);

	hDC = GetDC(hWnd);
	backBuffer = CreateCompatibleDC(hDC);
	ReleaseDC(hWnd, hDC);

	renderControl.createWindow(hInst, Render);

	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}


