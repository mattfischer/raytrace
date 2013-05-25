#include "App.hpp"

#include "Parse/Parser.hpp"
#include "RenderEngine.hpp"

#include <commctrl.h>

#define CLASSNAME "raytrace"

App *App::sInstance;

App::App()
{
	sInstance = this;
}

int App::run(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine, int iCmdShow)
{
	MSG msg;
	WNDCLASSEX wc;
	HDC hDC;

	InitCommonControls();

	AST *ast = Parse::Parser::parse("scene.txt");
	mScene = Object::Scene::fromAST(ast);

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = wndProcStub;
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

	mHWnd = CreateWindowEx((DWORD)0, CLASSNAME, "Raytrace", WS_POPUPWINDOW | WS_CAPTION | WS_VISIBLE, 10, 10, mRenderControl.settings().width, mRenderControl.settings().height, NULL, NULL, hInst, NULL);

	hDC = GetDC(mHWnd);
	mBackBuffer = CreateCompatibleDC(hDC);
	ReleaseDC(mHWnd, hDC);

	mRenderControl.createWindow(hInst, this);

	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

void App::onRenderDone()
{
	DWORD endTime = GetTickCount();
	mRenderControl.setRenderTime(endTime - mStartTime);
	KillTimer(mHWnd, 0);

	free(mBits);
	delete mEngine;
}

LRESULT CALLBACK App::wndProcStub(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	return sInstance->wndProc(hWnd, iMsg, wParam, lParam);
}

LRESULT CALLBACK App::wndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	int x, y;

	switch(iMsg)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_TIMER:
			SetDIBits(mBackBuffer, mBackBitmap, 0, mRenderControl.settings().height, mBits, &mBi, DIB_RGB_COLORS);
			InvalidateRect(hWnd, NULL, FALSE);
			SetTimer(hWnd, 0, 0, NULL);
			break;

		case WM_PAINT:
			BeginPaint(hWnd, &ps);

			BitBlt(ps.hdc, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top, mBackBuffer, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);

			EndPaint(hWnd, &ps);
			break;

		case WM_LBUTTONUP:
			mRenderControl.show();
			break;
	}

	return DefWindowProc(hWnd, iMsg, wParam, lParam);
}

void App::onStartRender()
{
	int width;
	int height;
	HGDIOBJ oldBitmap;
	HDC hDC;

	width = mRenderControl.settings().width;
	height = mRenderControl.settings().height;

	SetWindowPos(mHWnd, NULL, 0, 0, width, height, SWP_NOMOVE);
	hDC = GetDC(mHWnd);
	mBackBitmap = CreateCompatibleBitmap(hDC, width, height);
	ReleaseDC(mHWnd, hDC);
	oldBitmap = SelectObject(mBackBuffer, (HGDIOBJ)mBackBitmap);
	DeleteObject(oldBitmap);

	mBits = (BYTE*)malloc(width * height * 3);
	memset(mBits, 0, width * height * 3);
	mBi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	mBi.bmiHeader.biWidth = width;
	mBi.bmiHeader.biHeight = height;
	mBi.bmiHeader.biPlanes = 1;
	mBi.bmiHeader.biBitCount = 24;
	mBi.bmiHeader.biCompression = BI_RGB;

	mStartTime = GetTickCount();
	mEngine = new RenderEngine(mScene, mRenderControl.settings());
	mEngine->render(mBits, this);

	SetTimer(mHWnd, 0, 0, NULL);
}
