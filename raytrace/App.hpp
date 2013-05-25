#ifndef APP_HPP
#define APP_HPP

#include "RenderControlDlg.hpp"
#include "RenderEngine.hpp"
#include "Object/Scene.hpp"

#include <windows.h>

class App : public RenderEngine::Listener, public RenderControlDlg::Listener
{
public:
	App();

	int run(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine, int iCmdShow);

	void onRenderDone();
	void onStartRender();

private:
	static LRESULT CALLBACK wndProcStub(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

	static App *sInstance;

	LRESULT CALLBACK wndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

	HWND mHWnd;
	HDC mBackBuffer;
	RenderControlDlg mRenderControl;
	Object::Scene *mScene;
	unsigned char *mBits;
	HBITMAP mBackBitmap;
	BITMAPINFO mBi;
	DWORD mStartTime;
	RenderEngine *mEngine;
};

#endif