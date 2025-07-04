#pragma once
#include "Sprite.h"
#include <vector>
#include <array>

struct collisionBoxPoint {
	float x, y;
	bool colliding;
};

class CollisionBox
{
public:
	CollisionBox(string CollisionBoxName);
	~CollisionBox() {}
	
	void setupRectangularBox(SpriteSettings spriteSettings);
	void updateCollisionBox(SpriteSettings spriteSettings);
	void renderCollisionBox();
	PointXY CalculateCentreOfBox();
	array<collisionBoxPoint, 4>& getBoxCornerLocations() { return _boxCornerLocations; }
	array<collisionBoxPoint, 4>& getRotatedBoxLocations() { return _rotatedBoxLocations; }

private:
	collisionBoxPoint _boxCornersPercent[4];	
	Sprite* _cross[4];
	Sprite* _movedCross[4];
	Sprite* _centreCross;
	SpriteSettings _CBSettings;
	
	array<collisionBoxPoint, 4> _boxCornerLocations;
	array<collisionBoxPoint, 4> _rotatedBoxLocations;

	void UpdateBoxCornerLocations(SpriteSettings spriteSettings);
};