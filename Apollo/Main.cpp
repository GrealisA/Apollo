/************************************
* Project: Space Race				*	
* Year: 2017-18						*
* Creator: Thomas Alfred Grealis	*
* School: Epsom College				*
************************************/

//Reference For Basic Direct2D Application: https://msdn.microsoft.com/en-us/library/windows/desktop/dd370994(v=vs.85).aspx
#include <Windows.h>
#include <d2d1.h>
#include <iostream>
#include <thread>
#include "LevelManager.h"

using namespace std;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR cmd, int nCmdShow)
{
	Graphics* graphics = new Graphics(1280, 720);
	LevelManager* gameControl = new LevelManager(graphics);

	WNDCLASSEX ApolloWindow{};
	ApolloWindow.cbSize = sizeof(WNDCLASSEX);
	ApolloWindow.hbrBackground = (HBRUSH)COLOR_WINDOW;
	ApolloWindow.hInstance = hInstance;
	ApolloWindow.lpfnWndProc = WindowProc;
	ApolloWindow.lpszClassName = "MainWindow";
	ApolloWindow.style = CS_HREDRAW | CS_VREDRAW;

	if (!RegisterClassEx(&ApolloWindow)) return -1;
	RECT rect = { 0, 0, LONG(graphics->getWindowSize().sizeX), LONG(graphics->getWindowSize().sizeY)};
	AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, false, WS_EX_OVERLAPPEDWINDOW);
	DWORD style = WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX;

	HWND windowhandle = CreateWindowEx(WS_EX_CLIENTEDGE, "MainWindow", "Apollo", style, CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, hInstance, NULL);
	if (!windowhandle) return -2;
	//Reference: http://forums.codeguru.com/showthread.php?500841-Set-my-window-position-at-screen-center
	SetWindowPos(windowhandle, 0, (GetSystemMetrics(SM_CXSCREEN) - rect.right) / 2, (GetSystemMetrics(SM_CYSCREEN) - rect.bottom) / 2, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	//End Reference
	if (!graphics->initialiseGraphics(windowhandle))
	{
		delete graphics;
		return -1;
	}
	if (graphics->getRenderTarget() == nullptr) {
		delete graphics;
		return -1;
	}

	graphics->getRenderTarget()->Resize(D2D1::SizeU(1920, 1080));
	graphics->getWindowSize().sizeX = 1920;	graphics->getWindowSize().sizeY = 1080;

	gameControl->LoadInitialLevel(gameControl->getMainMenu());

	ShowWindow(windowhandle, nCmdShow);
	UpdateWindow(windowhandle);

	MSG message;
	message.message = WM_NULL;

	while (message.message != WM_QUIT)
	{
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) DispatchMessage(&message); 
		else {
			//Frame Updating and Rendering
			gameControl->UpdateLevel();
			graphics->BeginRender();
			gameControl->RenderLevel();
			graphics->EndRender();
		}
	}
	FreeConsole();
	delete graphics;
	return 0;
}
//End Reference