#include "LightProbeDlg.hpp"

#include "Lighter/Direct.hpp"
#include "Lighter/DiffuseIndirect.hpp"

#include <commctrl.h>

#include "Resource.h"

LightProbeDlg *LightProbeDlg::sInstance;

LightProbeDlg::LightProbeDlg()
{
	sInstance = this;
}

void LightProbeDlg::createWindow(HINSTANCE hInst)
{
	mHDlg = CreateDialog(hInst, (LPCSTR)IDD_LIGHT_PROBE, NULL, dialogProcStub);
}

bool LightProbeDlg::isDialogMessage(MSG *msg)
{
	return IsDialogMessage(mHDlg, msg);
}

void LightProbeDlg::show()
{
	ShowWindow(mHDlg, SW_SHOW);
}

INT_PTR CALLBACK LightProbeDlg::dialogProcStub(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return sInstance->dialogProc(hwndDlg, uMsg, wParam, lParam);
}

INT_PTR CALLBACK LightProbeDlg::dialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	char buf[20];

	switch (uMsg)
	{
	case WM_INITDIALOG:
		return FALSE;

	case WM_COMMAND:
		break;

	case WM_CLOSE:
		ShowWindow(hwndDlg, SW_HIDE);
		return TRUE;

	case WM_PAINT:
		{
		PAINTSTRUCT ps;
		BeginPaint(mHDlg, &ps);
		RECT rect;
		GetClientRect(mHDlg, &rect);
		HBRUSH brush = CreateSolidBrush(RGB(0x40, 0x40, 0x40));
		FillRect(ps.hdc, &rect, brush);
		DeleteObject(brush);
		for (Sample &sample : mSamples) {
			float size = (rect.right - rect.left);
			float radius = std::cos(sample.elevation) * size / 2;
			float x = std::cos(sample.azimuth) * radius + size / 2;
			float y = std::sin(sample.azimuth) * radius + size / 2;
			HBRUSH brush = CreateSolidBrush(RGB(sample.color.red() * 0xff, sample.color.green() * 0xff, sample.color.blue() * 0xff));
			HBRUSH oldBrush = (HBRUSH)SelectObject(ps.hdc, brush);
			Ellipse(ps.hdc, x - 5, y - 5, x + 5, y + 5);
			SelectObject(ps.hdc, oldBrush);
			DeleteObject(brush);
		}
		EndPaint(mHDlg, &ps);
		return TRUE;
		}
	}
	return FALSE;
}

void LightProbeDlg::renderProbe(Render::Engine &engine, int x, int y)
{
	Render::Sampler sampler(0);
	Math::Beam beam = engine.scene().camera().createPixelBeam(Math::Point2D(x, y), engine.settings().width, engine.settings().height, Math::Point2D());

	Object::Intersection intersection = engine.scene().intersect(beam);

	if (intersection.valid()) {
		Lighter::DiffuseIndirect lighter(engine.settings().lighterSettings.indirectSamples, engine.settings().lighterSettings.indirectDirectSamples, false, 0);
		//Trace::Lighter::Direct lighter(engine.settings().lighterSettings.directSamples, false, 0);

		mSamples.clear();

		Math::OrthonormalBasis basis(intersection.normal());
		for(int i=0; i<1000; i++) {
			Math::Vector localIncidentDirection;
			Object::Radiance irradiance = lighter.sampleIrradiance(intersection, basis, sampler, localIncidentDirection);
			Object::Color color = engine.toneMap(irradiance);
			float azimuth = std::atan2(localIncidentDirection.y(), localIncidentDirection.x());
			float elevation = std::asin(localIncidentDirection.z());
			mSamples.push_back(Sample{ color, azimuth, elevation });
		}
		InvalidateRect(mHDlg, NULL, TRUE);
	}
}