#ifndef RENDER_CONTROL_DLG_HPP
#define RENDER_CONTROL_DLG_HPP

#include <windows.h>

#include "Trace/Tracer.hpp"

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

	Trace::Tracer::Settings &settings();

	void setStatusMessage(const char *message);
	void enableRenderButton(bool enabled);

	void show();

private:
	static INT_PTR CALLBACK dialogProcStub(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static RenderControlDlg *sInstance;

	INT_PTR CALLBACK dialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	HWND mHDlg;
	Trace::Tracer::Settings mSettings;
	Listener *mListener;
};

#endif