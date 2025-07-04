#pragma once

#include <crtdbg.h>
#include <d2d1.h>
#include <dwrite.h>
#include <comdef.h>
#include "Graphics.h"

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

using namespace std;

struct textSettings {
	string text;
	string alignment;
	WCHAR* font;
	float fontSize;
	PointXY locationPercent;
	float opacity;
	float rotation;
	SizeXY textDimensionsPercent;
	D2D1::ColorF::Enum colour;
};

class Text {
public:

	Text(WCHAR* font, float fontSize, Graphics* gfx);
	~Text();

	void Render();
	textSettings& getTSettings() { return _tSettings; }
private:	
	//Reference: https://msdn.microsoft.com/en-us/library/windows/desktop/dd368152(v=vs.85).aspx
	ID2D1Factory* g_pD2DFactory = NULL;
	ID2D1SolidColorBrush* g_pSolidBrush = NULL;
	IDWriteFactory* g_pDWriteFactory = NULL;
	IDWriteTextFormat* g_pTextFormat = NULL;
	IDWriteTextLayout* g_pTextLayout = NULL;
	//End Reference

	Graphics* _gfx;	
	textSettings _tSettings;
};
