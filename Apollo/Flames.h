#pragma once
#include "SpriteSheet.h"

class Flames: public SpriteSheet {
public:
	Flames(int flameTypeNo) : SpriteSheet("SpriteSheet\\Flames\\Flame" + to_string(flameTypeNo) + ".png", "Flame" + to_string(flameTypeNo), 107, 178) {};
	~Flames() { SpriteSheet::~SpriteSheet(); }
};