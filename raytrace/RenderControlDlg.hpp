#ifndef RENDER_CONTROL_DLG_HPP
#define RENDER_CONTROL_DLG_HPP

#include <windows.h>

#include "Render/Settings.hpp"

class RenderControlDlg
{
public:
	class Listener
	{
	public:
		virtual void onRenderButtonClicked() = 0;
	};

	RenderControlDlg();

	void createWindow(HINSTANCE hInst, Listener *listener);
	bool isDialogMessage(MSG *msg);

	Render::Settings &settings();

	void setStatusMessage(const char *message);
	void enableRenderButton(bool enabled);

	void show();
	void refreshSettings();

private:
	static INT_PTR CALLBACK dialogProcStub(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static RenderControlDlg *sInstance;

	INT_PTR CALLBACK dialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	HWND mHDlg;
	Render::Settings mSettings;
	Listener *mListener;
};

#endif