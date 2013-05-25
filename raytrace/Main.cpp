#include <windows.h>

#include "App.hpp"

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine, int iCmdShow)
{
	App app;

	return app.run(hInst, hPrev, lpCmdLine, iCmdShow);
}


