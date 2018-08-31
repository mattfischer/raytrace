#ifndef LIGHT_PROBE_DLG_HPP
#define LIGHT_PROBE_DLG_HPP

#include <windows.h>

#include "Render/Engine.hpp"
#include "Object/Color.hpp"

class LightProbeDlg
{
public:
	LightProbeDlg();

	void createWindow(HINSTANCE hInst);
	bool isDialogMessage(MSG *msg);

	void show();

	void renderProbe(Render::Engine &engine, int x, int y);

private:
	static INT_PTR CALLBACK dialogProcStub(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LightProbeDlg *sInstance;

	INT_PTR CALLBACK dialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	struct Sample {
		Object::Color color;
		float azimuth;
		float elevation;
	};
	std::vector<Sample> mSamples;
	HWND mHDlg;
};
#endif