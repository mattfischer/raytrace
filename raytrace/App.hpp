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

	void onStartRender();
	void onRenderDone();

private:
	static LRESULT CALLBACK wndProcStub(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

	static App *sInstance;

	LRESULT CALLBACK wndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

	HWND mHWnd;
	HDC mBackDC;

	RenderControlDlg mRenderControl;
	Object::Scene *mScene;
	RenderEngine mEngine;
	unsigned char *mFramebuffer;
	DWORD mStartTime;
	bool mRendering;
};

#endif