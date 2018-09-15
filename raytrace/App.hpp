#ifndef APP_HPP
#define APP_HPP

#include "RenderControlDlg.hpp"
#include "LightProbeDlg.hpp"
#include "Render/Engine.hpp"
#include "Render/Framebuffer.hpp"
#include "Object/Scene.hpp"

#include <windows.h>

#include <memory>

class App : public Render::Engine::Listener, public RenderControlDlg::Listener
{
public:
	App();

	int run(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine, int iCmdShow);

	void onRenderButtonClicked();
	void onRenderStatus(const char *message);
	void onRenderDone();
	void onPrerenderDone();

private:
	static LRESULT CALLBACK wndProcStub(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

	static App *sInstance;

	LRESULT CALLBACK wndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

	HWND mHWnd;
	HDC mBackDC;

	RenderControlDlg mRenderControl;
	LightProbeDlg mLightProbe;
	std::unique_ptr<Object::Scene> mScene;
	std::unique_ptr<Render::Engine> mEngine;
	std::unique_ptr<Render::Framebuffer> mFramebuffer;
};

#endif