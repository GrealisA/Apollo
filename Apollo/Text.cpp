#include "Text.h"

Text::Text(WCHAR* font, float fontSize, Graphics* gfx) {
	_gfx = gfx;
	_tSettings.text = "";
	_tSettings.font = font;
	_tSettings.fontSize = fontSize;
	_tSettings.opacity = 1.0f;
	_tSettings.colour = D2D1::ColorF::Black;
	_tSettings.alignment = "Left";

	//Reference: https://msdn.microsoft.com/en-us/library/windows/desktop/dd368152(v=vs.85).aspx
	HRESULT hr = S_OK;
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_pD2DFactory);

	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&g_pDWriteFactory));

	hr = g_pDWriteFactory->CreateTextFormat(font, NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize*(_gfx->getWindowSize().sizeX/500), L"en-us", &g_pTextFormat);

	hr = g_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);

	IDWriteRenderingParams *oldparams = NULL;
	IDWriteRenderingParams *params = NULL;
	g_pDWriteFactory->CreateRenderingParams(&oldparams);
	g_pDWriteFactory->CreateCustomRenderingParams(oldparams->GetGamma(), oldparams->GetEnhancedContrast(), 0.0f, oldparams->GetPixelGeometry(), DWRITE_RENDERING_MODE_DEFAULT, &params);
	_gfx->getRenderTarget()->SetTextRenderingParams(params);
	//End Reference
}

Text::~Text() {
	if (g_pTextFormat) g_pTextFormat->Release();
	if (g_pDWriteFactory) g_pDWriteFactory->Release();
	if (g_pD2DFactory) g_pD2DFactory->Release();
	if (g_pTextLayout) g_pTextLayout->Release();
	if (g_pSolidBrush) g_pSolidBrush->Release();
}

void Text::Render() {
	//Text Conversions
	const char* text = _tSettings.text.c_str();
	SizeXY windowSize = _gfx->getWindowSize();
	
	//Reference: https://msdn.microsoft.com/en-us/library/windows/desktop/dd368152(v=vs.85).aspx	
	const WCHAR* wtext;
	int nChars = MultiByteToWideChar(CP_ACP, 0, text, -1, NULL, 0);
	wtext = new WCHAR[nChars];
	MultiByteToWideChar(CP_ACP, 0, text, -1, (LPWSTR)wtext, nChars);

	RECT rc;
	GetClientRect(_gfx->getHwnd(), &rc);

	_gfx->getRenderTarget()->CreateSolidColorBrush(D2D1::ColorF(_tSettings.colour), &g_pSolidBrush);
	g_pSolidBrush->SetOpacity(_tSettings.opacity);

	D2D1_SIZE_F sizeRT;
	sizeRT.width = (_tSettings.textDimensionsPercent.sizeX / 100) * windowSize.sizeX;
	sizeRT.height = (_tSettings.textDimensionsPercent.sizeY / 100) * windowSize.sizeY;

	g_pDWriteFactory->CreateTextFormat(_tSettings.font, NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, _tSettings.fontSize*(windowSize.sizeX / 500), L"en-us", &g_pTextFormat);

	if (_tSettings.alignment == "Left") g_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	else if (_tSettings.alignment == "Middle") g_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

	g_pDWriteFactory->CreateTextLayout(wtext, (UINT32)_tSettings.text.length(), g_pTextFormat, sizeRT.width, sizeRT.height, &g_pTextLayout);
	delete[] wtext;

	_gfx->getRenderTarget()->SetTransform(D2D1::IdentityMatrix());

	D2D1_POINT_2F origin = D2D1::Point2F((_tSettings.locationPercent.x /100) * windowSize.sizeX, (_tSettings.locationPercent.y / 100) * windowSize.sizeY);
	if (_tSettings.rotation != 0) _gfx->getRenderTarget()->SetTransform(D2D1::Matrix3x2F::Rotation(_tSettings.rotation, D2D1::Point2F(0.0f, 0.0f)));

	_gfx->getRenderTarget()->DrawTextLayout(origin, g_pTextLayout, g_pSolidBrush);
	//End Reference
}
