#include "App.hpp"

#include "Parse/Parser.hpp"

#include <commctrl.h>

#define CLASSNAME "raytrace"

App *App::sInstance;

App::App()
{
	sInstance = this;
}

int App::run(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine, int iCmdShow)
{
	InitCommonControls();

	AST *ast = Parse::Parser::parse("scene.txt");
	mScene = Object::Scene::fromAST(ast);
	mEngine = std::make_unique<Render::Engine>(*mScene);

	WNDCLASSEX wc;
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

	RECT rect;
	rect.left = 10;
	rect.top = 50;
	rect.right = rect.left + mRenderControl.settings().width;
	rect.bottom = rect.top + mRenderControl.settings().height;
	AdjustWindowRect(&rect, WS_POPUPWINDOW | WS_CAPTION, FALSE);
	mHWnd = CreateWindowEx((DWORD)0, CLASSNAME, "Raytrace", WS_POPUPWINDOW | WS_CAPTION | WS_VISIBLE, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, hInst, NULL);

	HDC hDC = GetDC(mHWnd);
	mBackDC = CreateCompatibleDC(hDC);
	ReleaseDC(mHWnd, hDC);

	mRenderControl.createWindow(hInst, this);
	mLightProbe.createWindow(hInst);

	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0))
	{
		if(mRenderControl.isDialogMessage(&msg)) {
			continue;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

LRESULT CALLBACK App::wndProcStub(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	return sInstance->wndProc(hWnd, iMsg, wParam, lParam);
}

LRESULT CALLBACK App::wndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch(iMsg)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_TIMER:
		{
			BITMAPINFO bi;
			bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bi.bmiHeader.biWidth = mRenderControl.settings().width;
			bi.bmiHeader.biHeight = mRenderControl.settings().height;
			bi.bmiHeader.biPlanes = 1;
			bi.bmiHeader.biBitCount = 24;
			bi.bmiHeader.biCompression = BI_RGB;

			HBITMAP hBitmap = (HBITMAP)GetCurrentObject(mBackDC, OBJ_BITMAP);
			SetDIBits(mBackDC, hBitmap, 0, mRenderControl.settings().height, mEngine->framebuffer().bits(), &bi, DIB_RGB_COLORS);
			InvalidateRect(hWnd, NULL, FALSE);

			if(mEngine->rendering()) {
				SetTimer(hWnd, 0, 0, NULL);
			}
			break;
		}

		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			BeginPaint(hWnd, &ps);

			BitBlt(ps.hdc, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top, mBackDC, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);

			EndPaint(hWnd, &ps);
			break;
		}

		case WM_LBUTTONUP:
			mRenderControl.show();
			mRenderControl.refreshSettings();
			mEngine->setSettings(mRenderControl.settings());
			updateFramebuffer();

			mLightProbe.show();
			mLightProbe.renderProbe(*mEngine, LOWORD(lParam), HIWORD(lParam));
			break;
	}

	return DefWindowProc(hWnd, iMsg, wParam, lParam);
}

void App::onRenderButtonClicked()
{
	updateFramebuffer();

	mRenderControl.enableRenderButton(false);

	mEngine->setSettings(mRenderControl.settings());
	mEngine->startRender(this);

	SetTimer(mHWnd, 0, 0, NULL);
}

void App::onRenderDone()
{
	mRenderControl.enableRenderButton(true);
}

void App::onRenderStatus(const char *message)
{
	mRenderControl.setStatusMessage(message);
}

void App::updateFramebuffer()
{
	int width = mRenderControl.settings().width;
	int height = mRenderControl.settings().height;

	RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = width;
	rect.bottom = height;
	AdjustWindowRect(&rect, WS_POPUPWINDOW | WS_CAPTION, FALSE);

	SetWindowPos(mHWnd, NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE);
	HDC hDC = GetDC(mHWnd);
	HBITMAP hBitmap = CreateCompatibleBitmap(hDC, width, height);
	ReleaseDC(mHWnd, hDC);
	HGDIOBJ oldBitmap = SelectObject(mBackDC, (HGDIOBJ)hBitmap);
	DeleteObject(oldBitmap);
}