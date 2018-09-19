#include "LightProbeDlg.hpp"

#include "Trace/Lighter/Direct.hpp"
#include "Trace/Lighter/Indirect.hpp"

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
	std::unique_ptr<Trace::Tracer> tracer = engine.createTracer();
	Trace::Ray ray = tracer->createCameraRay(x, y);
	Trace::IntersectionVector::iterator begin, end;

	tracer->intersect(ray, begin, end);
	if (begin != end) {
		Trace::Intersection intersection = *begin;

		Trace::Lighter::Indirect lighter(engine.settings().indirectSamples, engine.settings().indirectDirectSamples);
		//Trace::Lighter::Direct lighter(engine.settings().directSamples);
		std::vector<Trace::Lighter::Base::ProbeEntry> probe;
		lighter.setProbe(&probe);
		lighter.sampleHemisphere(intersection, *tracer);

		mSamples.clear();
		for (Trace::Lighter::Base::ProbeEntry &entry : probe) {
			Object::Color color = tracer->toneMap(entry.radiance);
			float azimuth = std::atan2(entry.direction.y(), entry.direction.x());
			float elevation = std::asin(entry.direction.z());
			mSamples.push_back(Sample{ color, azimuth, elevation });
		}
		InvalidateRect(mHDlg, NULL, TRUE);
	}
}