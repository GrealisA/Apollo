#include "Sprite.h"

Graphics* Sprite::_gfx;

//Converts the string to a wide string so it can be used for system procedures
wstring stringToWString(string text) {
	//Reference: https://stackoverflow.com/questions/2573834/c-convert-string-or-char-to-wstring-or-wchar-t
	wstring tempString(text.length(), L' ');
	copy(text.begin(), text.end(), tempString.begin());
	//End Reference
	return tempString;
}

Sprite::Sprite(string filename, string spriteName) {
	std::cout << "Sprite " << spriteName << " Initialisation Started\n";
	_sSettings.spriteName = spriteName;
	_sSettings.rotationMatrix = D2D1::Matrix3x2F::Identity();
	_sSettings.opacity = 1;
	bmp = NULL;
	wstring tempFilename = stringToWString(filename);
	const wchar_t* filenameWChar = tempFilename.c_str();
	if (filename != "") {
		try{	
			//Reference: https://msdn.microsoft.com/en-us/library/windows/desktop/dd756686(v=vs.85).aspx
			HRESULT hr;
			CoInitializeEx(NULL, COINIT_MULTITHREADED);
			IWICImagingFactory *wicFactory = NULL;
			hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicFactory));
			IWICBitmapDecoder *wicDecoder = NULL;
			hr = wicFactory->CreateDecoderFromFilename(filenameWChar, NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &wicDecoder);
			if(wicDecoder == nullptr) throw filename;

			IWICBitmapFrameDecode* wicFrame = NULL;
			hr = wicDecoder->GetFrame(0, &wicFrame);

			IWICFormatConverter *wicConverter = NULL;
			hr = wicFactory->CreateFormatConverter(&wicConverter);

			hr = wicConverter->Initialize(wicFrame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0, WICBitmapPaletteTypeCustom);

			_gfx->getRenderTarget()->CreateBitmapFromWicBitmap(wicConverter, NULL, &bmp);
			if (wicFactory) wicFactory->Release();
			if (wicDecoder) wicDecoder->Release();
			if (wicConverter) wicConverter->Release();
			if (wicFrame) wicFrame->Release();
			//End Reference
		}
		catch (string filename) {
			cout << "Sprite Loading error at " << filename << endl;
			system("pause");
		}
	}
	std::cout << "Sprite " << spriteName << " Initialisation Finished\n";
}

Sprite::~Sprite() {
	std::cout << "Sprite " << _sSettings.locationPercent.x << " deleted\n";
	if (bmp) bmp->Release();
}

void Sprite::Render() {
	float x, y;
	//Converts the percentage locations of the sprite on the screen to pixels
	float sizeX = (_sSettings.spriteDimensionsPercent.sizeX / 100) * _gfx->getWindowSize().sizeX;
	float sizeY = (_sSettings.spriteDimensionsPercent.sizeY / 100) * _gfx->getWindowSize().sizeY;
	
	if (_sSettings.angleFV == 0) {
		x = (_sSettings.locationPercent.x / 100) * _gfx->getWindowSize().sizeX;
		y = (_sSettings.locationPercent.y / 100) * _gfx->getWindowSize().sizeY;
	}
	else {
		x = (_sSettings.rotatedLocationPercent.x / 100) * _gfx->getWindowSize().sizeX;
		y = (_sSettings.rotatedLocationPercent.y / 100) * _gfx->getWindowSize().sizeY;
	}
	//Rotates the render window to match the rotation of the sprite
	_gfx->getRenderTarget()->SetTransform(_sSettings.rotationMatrix);
	if (bmp != nullptr) {
		_gfx->getRenderTarget()->DrawBitmap(bmp, D2D1::RectF(x, y, x + sizeX, y + sizeY), _sSettings.opacity, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, D2D1::RectF(0.0f, 0.0f, bmp->GetSize().width, bmp->GetSize().height));
	}
	//Resets the rotation of the render window
	_gfx->getRenderTarget()->SetTransform(D2D1::IdentityMatrix());
}

void Sprite::Rotate(float angle, PointXY centrePoint) {
	//Rotates the sprite
	_sSettings.rotationMatrix = _sSettings.rotationMatrix * D2D1::Matrix3x2F::Rotation(angle, D2D1::Point2F(centrePoint.x, centrePoint.y));
	_sSettings.angleFV += angle;
	if (_sSettings.angleFV >= 360) _sSettings.angleFV -= 360;
	else if (_sSettings.angleFV < 0) _sSettings.angleFV += 360;
}

void Sprite::setScalePercent(float newScalePercent) {
	_sSettings.scalePercent = newScalePercent;
	_sSettings.spriteDimensionsPercent.sizeX = _sSettings.scalePercent;

	//Gets the number of pixels x represents and maintains the aspect ratio while converting that to a y percent, as x percent and y percent dont have a 1:1 ratio 
	_sSettings.spriteDimensionsPercent.sizeY = (_sSettings.spriteDimensionsPercent.sizeX * _gfx->getWindowSize().sizeX * (this->getBMPSize().height / this->getBMPSize().width)) / _gfx->getWindowSize().sizeY;
}