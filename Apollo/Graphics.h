#pragma once
#include <Windows.h>
#include <d2d1.h>
#include <string>
#include <iostream>
#include "DataTypesAndConstants.h"
#include "Sound.h"

using namespace std;
//Reference: https://www.youtube.com/watch?v=ng9Ho9qcVII
class Graphics
{
public:
	Graphics(int xResolution, int yResolution);
	~Graphics();

	bool initialiseGraphics(HWND windowHandle);

	void BeginRender() { _renderTarget->BeginDraw(); };
	void EndRender() { _renderTarget->EndDraw(); };

	void ClearScreen(float r, float g, float b, float a);
	void ClearScreen(D2D1::ColorF clearColour);
	
	SizeXY& getWindowSize() { return _windowSize; }
	PointXY getCentreOfScreen() { _pixelCentreOfScreen.x = _windowSize.sizeX / 2; _pixelCentreOfScreen.y = _windowSize.sizeY / 2; return _pixelCentreOfScreen; }

	ID2D1HwndRenderTarget* getRenderTarget() { return _renderTarget; }
	HWND getHwnd() { return _windowHandle; }
private:
	HWND _windowHandle;
	ID2D1Factory* _factory;
	ID2D1HwndRenderTarget* _renderTarget;
	SizeXY _windowSize;
	PointXY _pixelCentreOfScreen;
};
//End Reference