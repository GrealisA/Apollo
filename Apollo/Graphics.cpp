#include "Graphics.h"

//Reference: https://www.youtube.com/watch?v=ng9Ho9qcVII
Graphics::Graphics(int xResolution, int yResolution)
{
	_factory = NULL;
	_renderTarget = NULL;
	_windowSize.sizeX = float(xResolution);
	_windowSize.sizeY = float(yResolution);
	cout << "Graphics Loaded\n";
}

Graphics::~Graphics() //Deconstructor
{
	if (_factory) _factory->Release();
	if (_renderTarget) _renderTarget->Release();
	cout << "Graphics Unloaded\n";
}

bool Graphics::initialiseGraphics(HWND windowHandle)
{
	_windowHandle = windowHandle;
	HRESULT res = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &_factory);
	if (res != S_OK) return false;
	RECT rect;
	GetClientRect(windowHandle, &rect);

	_factory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(windowHandle, D2D1::SizeU(rect.right, rect.bottom)), &_renderTarget);
	
	if (res != S_OK) return false;
	if (_renderTarget != nullptr) {
		cout << "Graphics Initialised\n";
		return true;
	}
	else {
		cout << "Graphics Initialisation Failed\n";
		return false;
	}
}

void Graphics::ClearScreen(float r, float g, float b, float a)
{
	r /= 255; g /= 255; b /= 255;
	_renderTarget->Clear(D2D1::ColorF(r, g, b, a));
}

void Graphics::ClearScreen(D2D1::ColorF clearColour) {
	clearColour.r /= 255;
	clearColour.g /= 255;
	clearColour.b /= 255;
	_renderTarget->Clear(clearColour);
}
//End Reference