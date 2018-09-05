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
	mSettings.antialiasSamples = 9;
	mSettings.radiantLighting = true;
	mSettings.specularLighting = true;
	mSettings.directLighting = true;
	mSettings.directSamples = 10;
	mSettings.indirectLighting = true;
	mSettings.indirectSamples = 1000;
	mSettings.indirectDirectSamples = 10;
	mSettings.indirectCacheThreshold = 0.15;
}

void RenderControlDlg::createWindow(HINSTANCE hInst, Listener *listener)
{
	mListener = listener;
	mHDlg = CreateDialog(hInst, (LPCSTR)IDD_RENDER_CONTROL, NULL, dialogProcStub);
	SetWindowPos(mHDlg, NULL, mSettings.width + 40, 40, 0, 0, SWP_NOSIZE);
	ShowWindow(mHDlg, SW_SHOW);
}

bool RenderControlDlg::isDialogMessage(MSG *msg)
{
	return IsDialogMessage(mHDlg, msg);
}

Trace::Tracer::Settings &RenderControlDlg::settings()
{
	return mSettings;
}

void RenderControlDlg::setStatusMessage(const char *message)
{
	SetDlgItemText(mHDlg, IDC_RENDER_STATUS, message);
}

void RenderControlDlg::enableRenderButton(bool enabled)
{
	EnableWindow(GetDlgItem(mHDlg, ID_RENDER), enabled);
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
	char buf[20];

	switch(uMsg)
	{
	case WM_INITDIALOG:
		SetDlgItemInt(hwndDlg, IDC_WIDTH, mSettings.width, TRUE);
		SetDlgItemInt(hwndDlg, IDC_HEIGHT, mSettings.height, TRUE);
		CheckDlgButton(hwndDlg, IDC_LIGHTING, mSettings.lighting ? BST_CHECKED : BST_UNCHECKED);
		SetDlgItemInt(hwndDlg, IDC_AA_SAMPLES, mSettings.antialiasSamples, TRUE);
		SendDlgItemMessage(hwndDlg, IDC_MAX_GEN_SPIN, UDM_SETRANGE, 0, MAKELPARAM(20, 1));
		CheckDlgButton(hwndDlg, IDC_RADIANT, mSettings.radiantLighting ? BST_CHECKED : BST_UNCHECKED);
		CheckDlgButton(hwndDlg, IDC_SPECULAR, mSettings.specularLighting ? BST_CHECKED : BST_UNCHECKED);
		CheckDlgButton(hwndDlg, IDC_DIRECT, mSettings.directLighting ? BST_CHECKED : BST_UNCHECKED);
		SetDlgItemInt(hwndDlg, IDC_DIRECT_SAMPLES, mSettings.directSamples, TRUE);
		CheckDlgButton(hwndDlg, IDC_INDIRECT, mSettings.indirectLighting ? BST_CHECKED : BST_UNCHECKED);
		SetDlgItemInt(hwndDlg, IDC_INDIRECT_SAMPLES, mSettings.indirectSamples, TRUE);
		SetDlgItemInt(hwndDlg, IDC_INDIRECT_DIRECT_SAMPLES, mSettings.indirectDirectSamples, TRUE);
		sprintf_s(buf, sizeof(buf), "%f", mSettings.indirectCacheThreshold);
		SetDlgItemText(hwndDlg, IDC_INDIRECT_CACHE_THRESHOLD, buf);
		return FALSE;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case ID_RENDER:
			refreshSettings();
			mListener->onRenderButtonClicked();
			return TRUE;
		}
		break;

	case WM_CLOSE:
		ShowWindow(hwndDlg, SW_HIDE);
		return TRUE;
	}
	return FALSE;
}

void RenderControlDlg::refreshSettings()
{
	char buf[20];

	mSettings.width = GetDlgItemInt(mHDlg, IDC_WIDTH, NULL, TRUE);
	mSettings.height = GetDlgItemInt(mHDlg, IDC_HEIGHT, NULL, TRUE);
	mSettings.lighting = IsDlgButtonChecked(mHDlg, IDC_LIGHTING);
	mSettings.antialiasSamples = GetDlgItemInt(mHDlg, IDC_AA_SAMPLES, NULL, TRUE);
	mSettings.radiantLighting = IsDlgButtonChecked(mHDlg, IDC_RADIANT);
	mSettings.specularLighting = IsDlgButtonChecked(mHDlg, IDC_SPECULAR);
	mSettings.directLighting = IsDlgButtonChecked(mHDlg, IDC_DIRECT);
	mSettings.directSamples = GetDlgItemInt(mHDlg, IDC_DIRECT_SAMPLES, NULL, TRUE);
	mSettings.indirectLighting = IsDlgButtonChecked(mHDlg, IDC_INDIRECT);
	mSettings.indirectSamples = GetDlgItemInt(mHDlg, IDC_INDIRECT_SAMPLES, NULL, TRUE);
	mSettings.indirectDirectSamples = GetDlgItemInt(mHDlg, IDC_INDIRECT_DIRECT_SAMPLES, NULL, TRUE);
	GetDlgItemText(mHDlg, IDC_INDIRECT_CACHE_THRESHOLD, buf, sizeof(buf));
	mSettings.indirectCacheThreshold = atof(buf);
}