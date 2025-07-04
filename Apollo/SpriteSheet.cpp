#include "SpriteSheet.h"

SpriteSheet::SpriteSheet(string filename, string spriteName, int frameWidth, int frameHeight) : Sprite(filename, spriteName) {
	//Initialises the Spritesheet
	_frameHeight = frameHeight;
	_frameWidth = frameWidth;
	_framesPerRow = (int)bmp->GetSize().width / (int)frameWidth;
}


SpriteSheet::~SpriteSheet()
{
}

void SpriteSheet::Render(int index) {
	float x, y;
	//Converts the percentage locations of the spritesheet on the screen to pixels
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


	index %= getNoOfFrames();
	//Selects the current frame on the spritesheet
	D2D_RECT_F src = D2D1::RectF((float)((index % _framesPerRow) * _frameWidth), (float)((index / _framesPerRow) * _frameHeight), (float)((index % _framesPerRow) * _frameWidth) + _frameWidth, float(int((index / _framesPerRow) * _frameHeight)) + _frameHeight);
	D2D_RECT_F dest = D2D1::RectF(x, y, x + sizeX, y + sizeY);

	//Rotates the render window to match the rotation of the spritesheet
	_gfx->getRenderTarget()->SetTransform(_sSettings.rotationMatrix);
	if (bmp != nullptr) {
		_gfx->getRenderTarget()->DrawBitmap(bmp, dest, _sSettings.opacity, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, src);
	}
	//Resets the rotation of the render window
	_gfx->getRenderTarget()->SetTransform(D2D1::IdentityMatrix());
}

void SpriteSheet::setScalePercent(float newScalePercent) {
	_sSettings.scalePercent = newScalePercent;
	_sSettings.spriteDimensionsPercent.sizeX = _sSettings.scalePercent;

	//Gets the number of pixels x represents and maintains the aspect ratio while converting that to a y percent, as x percent and y percent dont have a 1:1 ratio 
	_sSettings.spriteDimensionsPercent.sizeY = (_sSettings.spriteDimensionsPercent.sizeX  * getNoOfFrames() * _gfx->getWindowSize().sizeX * (this->getBMPSize().height / this->getBMPSize().width)) / _gfx->getWindowSize().sizeY;
}