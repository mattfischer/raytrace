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
	mSettings.antialiasSamples = 3;
	mSettings.lighterSettings.radiantLighting = true;
	mSettings.lighterSettings.specularLighting = true;
	mSettings.lighterSettings.specularSamples = 4;
	mSettings.lighterSettings.specularMaxGeneration = 2;
	mSettings.lighterSettings.directLighting = true;
	mSettings.lighterSettings.directSamples = 32;
	mSettings.lighterSettings.indirectLighting = true;
	mSettings.lighterSettings.indirectSamples = 1000;
	mSettings.lighterSettings.indirectDirectSamples = 10;
	mSettings.lighterSettings.irradianceCaching = true;
	mSettings.lighterSettings.irradianceCacheThreshold = 0.1;
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

Render::Engine::Settings &RenderControlDlg::settings()
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
		CheckDlgButton(hwndDlg, IDC_RADIANT, mSettings.lighterSettings.radiantLighting ? BST_CHECKED : BST_UNCHECKED);
		CheckDlgButton(hwndDlg, IDC_SPECULAR, mSettings.lighterSettings.specularLighting ? BST_CHECKED : BST_UNCHECKED);
		SetDlgItemInt(hwndDlg, IDC_SPECULAR_SAMPLES, mSettings.lighterSettings.specularSamples, TRUE);
		SetDlgItemInt(hwndDlg, IDC_SPECULAR_MAX_GENERATION, mSettings.lighterSettings.specularMaxGeneration, TRUE);
		CheckDlgButton(hwndDlg, IDC_DIRECT, mSettings.lighterSettings.directLighting ? BST_CHECKED : BST_UNCHECKED);
		SetDlgItemInt(hwndDlg, IDC_DIRECT_SAMPLES, mSettings.lighterSettings.directSamples, TRUE);
		CheckDlgButton(hwndDlg, IDC_INDIRECT, mSettings.lighterSettings.indirectLighting ? BST_CHECKED : BST_UNCHECKED);
		SetDlgItemInt(hwndDlg, IDC_INDIRECT_SAMPLES, mSettings.lighterSettings.indirectSamples, TRUE);
		SetDlgItemInt(hwndDlg, IDC_INDIRECT_DIRECT_SAMPLES, mSettings.lighterSettings.indirectDirectSamples, TRUE);
		CheckDlgButton(hwndDlg, IDC_IRRADIANCE_CACHING, mSettings.lighterSettings.irradianceCaching ? BST_CHECKED : BST_UNCHECKED);
		sprintf_s(buf, sizeof(buf), "%f", mSettings.lighterSettings.irradianceCacheThreshold);
		SetDlgItemText(hwndDlg, IDC_IRRADIANCE_CACHE_THRESHOLD, buf);
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
	mSettings.lighterSettings.radiantLighting = IsDlgButtonChecked(mHDlg, IDC_RADIANT);
	mSettings.lighterSettings.specularLighting = IsDlgButtonChecked(mHDlg, IDC_SPECULAR);
	mSettings.lighterSettings.specularSamples = GetDlgItemInt(mHDlg, IDC_SPECULAR_SAMPLES, NULL, TRUE);
	mSettings.lighterSettings.specularMaxGeneration = GetDlgItemInt(mHDlg, IDC_SPECULAR_MAX_GENERATION, NULL, TRUE);
	mSettings.lighterSettings.directLighting = IsDlgButtonChecked(mHDlg, IDC_DIRECT);
	mSettings.lighterSettings.directSamples = GetDlgItemInt(mHDlg, IDC_DIRECT_SAMPLES, NULL, TRUE);
	mSettings.lighterSettings.indirectLighting = IsDlgButtonChecked(mHDlg, IDC_INDIRECT);
	mSettings.lighterSettings.indirectSamples = GetDlgItemInt(mHDlg, IDC_INDIRECT_SAMPLES, NULL, TRUE);
	mSettings.lighterSettings.indirectDirectSamples = GetDlgItemInt(mHDlg, IDC_INDIRECT_DIRECT_SAMPLES, NULL, TRUE);
	mSettings.lighterSettings.irradianceCaching = IsDlgButtonChecked(mHDlg, IDC_IRRADIANCE_CACHING);
	GetDlgItemText(mHDlg, IDC_IRRADIANCE_CACHE_THRESHOLD, buf, sizeof(buf));
	mSettings.lighterSettings.irradianceCacheThreshold = atof(buf);
}