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
	mSettings.threshold = 0.25f;
	mSettings.maxAAGen = 3;
	mSettings.radiantLighting = true;
	mSettings.specularLighting = true;
	mSettings.directLighting = true;
	mSettings.directSamples = 100;
	mSettings.indirectLighting = true;
	mSettings.indirectSamples = 25;
	mSettings.indirectDirectSamples = 10;
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
		sprintf_s(buf, sizeof(buf), "%f", mSettings.threshold);
		SetDlgItemText(hwndDlg, IDC_THRESHOLD, buf);
		SetDlgItemInt(hwndDlg, IDC_MAX_GEN, mSettings.maxAAGen, TRUE);
		SendDlgItemMessage(hwndDlg, IDC_MAX_GEN_SPIN, UDM_SETRANGE, 0, MAKELPARAM(20, 1));
		CheckDlgButton(hwndDlg, IDC_RADIANT, mSettings.radiantLighting ? BST_CHECKED : BST_UNCHECKED);
		CheckDlgButton(hwndDlg, IDC_SPECULAR, mSettings.specularLighting ? BST_CHECKED : BST_UNCHECKED);
		CheckDlgButton(hwndDlg, IDC_DIRECT, mSettings.directLighting ? BST_CHECKED : BST_UNCHECKED);
		SetDlgItemInt(hwndDlg, IDC_DIRECT_SAMPLES, mSettings.directSamples, TRUE);
		CheckDlgButton(hwndDlg, IDC_INDIRECT, mSettings.indirectLighting ? BST_CHECKED : BST_UNCHECKED);
		SetDlgItemInt(hwndDlg, IDC_INDIRECT_SAMPLES, mSettings.indirectSamples, TRUE);
		SetDlgItemInt(hwndDlg, IDC_INDIRECT_DIRECT_SAMPLES, mSettings.indirectDirectSamples, TRUE);
		return FALSE;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case ID_RENDER:
			mSettings.width = GetDlgItemInt(hwndDlg, IDC_WIDTH, NULL, TRUE);
			mSettings.height = GetDlgItemInt(hwndDlg, IDC_HEIGHT, NULL, TRUE);
			mSettings.lighting = IsDlgButtonChecked(hwndDlg, IDC_LIGHTING);
			GetDlgItemText(hwndDlg, IDC_THRESHOLD, buf, sizeof(buf));
			mSettings.threshold = atof(buf);
			mSettings.maxAAGen = GetDlgItemInt(hwndDlg, IDC_MAX_GEN, NULL, TRUE);
			mSettings.radiantLighting = IsDlgButtonChecked(hwndDlg, IDC_RADIANT);
			mSettings.specularLighting = IsDlgButtonChecked(hwndDlg, IDC_SPECULAR);
			mSettings.directLighting = IsDlgButtonChecked(hwndDlg, IDC_DIRECT);
			mSettings.directSamples = GetDlgItemInt(hwndDlg, IDC_DIRECT_SAMPLES, NULL, TRUE);
			mSettings.indirectLighting = IsDlgButtonChecked(hwndDlg, IDC_INDIRECT);
			mSettings.indirectSamples = GetDlgItemInt(hwndDlg, IDC_INDIRECT_SAMPLES, NULL, TRUE);
			mSettings.indirectDirectSamples = GetDlgItemInt(hwndDlg, IDC_INDIRECT_DIRECT_SAMPLES, NULL, TRUE);
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
