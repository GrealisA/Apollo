#pragma once
#include <wincodec.h>
#include "Graphics.h"
#pragma comment(lib, "windowscodecs.lib")

using namespace std;

struct SpriteSettings {
	string spriteName;
	PointXY locationPercent;
	PointXY rotatedLocationPercent;
	SizeXY spriteDimensionsPercent;
	float scalePercent, angleFV, opacity;
	D2D1::Matrix3x2F rotationMatrix;
};

class Sprite
{
public:
	Sprite(string filename, string spriteName);
	~Sprite();

	D2D1_SIZE_F getBMPSize() { return bmp->GetSize(); }
	static void setGFX(Graphics* gfx) { _gfx = gfx; }
	void Render();
	void Rotate(float angle, PointXY centrePoint);
	void setScalePercent(float newScalePercent);

	SpriteSettings& getSpriteSettings() { return _sSettings; }
protected:	
	SpriteSettings _sSettings;
	ID2D1Bitmap* bmp;
	static Graphics* _gfx;
};