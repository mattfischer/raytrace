#include "LightProbeDlg.hpp"

#include "Trace/Lighter/Direct.hpp"

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
		FillRect(ps.hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
		for (Sample &sample : mSamples) {
			float size = (rect.right - rect.left);
			float radius = std::cos(sample.elevation) * size / 2;
			float x = std::cos(sample.azimuth) * radius + size / 2;
			float y = std::sin(sample.azimuth) * radius + size / 2;
			HBRUSH brush = CreateSolidBrush(RGB(sample.color.red() * 0xff, sample.color.green() * 0xff, sample.color.blue() * 0xff));
			HBRUSH oldBrush = (HBRUSH)SelectObject(ps.hdc, brush);
			Ellipse(ps.hdc, x - 2, y - 2, x + 2, y + 2);
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
	Trace::Tracer tracer = engine.createTracer();
	Trace::Ray ray = tracer.createCameraRay(x, y);
	Trace::IntersectionVector::iterator begin, end;

	tracer.intersect(ray, begin, end);
	if (begin != end) {
		Trace::Intersection intersection = *begin;

		Trace::Lighter::Direct direct(engine.settings().directSamples);
		std::vector<Trace::Lighter::Base::ProbeEntry> probe;
		direct.setProbe(&probe);
		direct.light(intersection, tracer);

		mSamples.clear();
		for (Trace::Lighter::Base::ProbeEntry &entry : probe) {
			Object::Color color = tracer.toneMap(entry.radiance);
			float azimuth = std::atan2(entry.direction.y(), entry.direction.x());
			float elevation = std::asin(entry.direction.z());
			mSamples.push_back(Sample{ color, azimuth, elevation });
		}
		InvalidateRect(mHDlg, NULL, TRUE);
	}
}