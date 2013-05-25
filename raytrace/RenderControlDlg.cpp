#include "RenderControlDlg.hpp"

#include <commctrl.h>

#include "Resource.h"

RenderControlDlg *RenderControlDlg::sInstance;

RenderControlDlg::RenderControlDlg()
{
	sInstance = this;

	mSettings.width = 800;
	mSettings.height = 600;
	mSettings.lighting = true;
	mSettings.maxRayGeneration = 2;
	mSettings.antialias = 1;
}

void RenderControlDlg::createWindow(HINSTANCE hInst, StartRenderFunc startRenderFunc)
{
	mStartRenderFunc = startRenderFunc;
	mHDlg = CreateDialog(hInst, (LPCSTR)IDD_RENDER_CONTROL, NULL, dialogProcStub);
	SetWindowPos(mHDlg, NULL, mSettings.width + 40, 40, 0, 0, SWP_NOSIZE);
	ShowWindow(mHDlg, SW_SHOW);
}

Trace::Tracer::Settings &RenderControlDlg::settings()
{
	return mSettings;
}

void RenderControlDlg::setRenderTime(int time)
{
	char buf[256];
	sprintf(buf, "Render time: %ims", time);
	SetDlgItemText(mHDlg, IDC_RENDER_TIME, buf);
}

void RenderControlDlg::show()
{
	ShowWindow(mHDlg, SW_SHOW);
}

INT_PTR CALLBACK RenderControlDlg::dialogProcStub(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return sInstance->dialogProc(hwndDlg, uMsg, wParam, lParam);
}

INT_PTR CALLBACK RenderControlDlg::dialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		CheckDlgButton(hwndDlg, IDC_LIGHTING, mSettings.lighting ? BST_CHECKED : BST_UNCHECKED);
		SetDlgItemInt(hwndDlg, IDC_ANTIALIAS, mSettings.antialias, TRUE);
		SendDlgItemMessage(hwndDlg, IDC_ANTIALIAS_SPIN, UDM_SETRANGE, 0, MAKELPARAM(20, 1));
		SetDlgItemInt(hwndDlg, IDC_WIDTH, mSettings.width, TRUE);
		SetDlgItemInt(hwndDlg, IDC_HEIGHT, mSettings.height, TRUE);
		return FALSE;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case ID_RENDER:
			mSettings.lighting = IsDlgButtonChecked(hwndDlg, IDC_LIGHTING);
			mSettings.width = GetDlgItemInt(hwndDlg, IDC_WIDTH, NULL, TRUE);
			mSettings.height = GetDlgItemInt(hwndDlg, IDC_HEIGHT, NULL, TRUE);
			mSettings.antialias = GetDlgItemInt(hwndDlg, IDC_ANTIALIAS, NULL, TRUE);
			EnableWindow(GetDlgItem(hwndDlg, ID_RENDER), FALSE);
			SetDlgItemText(hwndDlg, IDC_RENDER_TIME, "");
			mStartRenderFunc();
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
