#pragma once
#include "SpriteSheet.h"

class Explosion : public SpriteSheet {
public:
	Explosion(int explosionTypeNo, PointXY crashLocation);
	~Explosion() { SpriteSheet::~SpriteSheet(); }
	void Render();

private:
	int _currentFrame;
	PointXY _originalLocation;
	Sound* _explosionSoundFX;
	static bool _explosionOccuring;
};
