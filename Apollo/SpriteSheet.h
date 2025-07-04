#pragma once
#include "Sprite.h"
class SpriteSheet : public Sprite
{
public:
	SpriteSheet(string filename, string spriteName, int frameWidth, int frameHeight);
	~SpriteSheet();
	void Render(int index);

	int getNoOfFrames() { return _framesPerRow * (int)(bmp->GetSize().height / _frameHeight); }
	int getNoOfFramesRow() { return _framesPerRow; }
	void setScalePercent(float newScalePercent);
private:
	int _frameWidth, _frameHeight;
	int _framesPerRow;
};

