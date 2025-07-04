#include "CollisionBox.h"

CollisionBox::CollisionBox(string CollisionBoxName) {
	_CBSettings.spriteName = CollisionBoxName;
}

void CollisionBox::setupRectangularBox(SpriteSettings spriteSettings) {
	//Distances between points are decided relative to the sprite 
	_boxCornersPercent[0].x = 0; _boxCornersPercent[0].y = 0;
	_boxCornersPercent[1].x = spriteSettings.spriteDimensionsPercent.sizeX; _boxCornersPercent[1].y = 0;
	_boxCornersPercent[2].x = 0; _boxCornersPercent[2].y = spriteSettings.spriteDimensionsPercent.sizeY;
	_boxCornersPercent[3].x = spriteSettings.spriteDimensionsPercent.sizeX; _boxCornersPercent[3].y = spriteSettings.spriteDimensionsPercent.sizeY;
	_CBSettings.scalePercent = spriteSettings.scalePercent;

	//Initialises the sprites for the indicators of the rocket in collision box mode
	if (collisionBoxMode) {
		for (int count = 0; count < 4; count++) {
			_cross[count] = new Sprite("Sprites\\cross" + to_string(count) + ".png", _CBSettings.spriteName + " Point " + to_string(count));
			_movedCross[count] = new Sprite("Sprites\\cross1.png", _CBSettings.spriteName + " Moved Point " + to_string(count));
		}
		_centreCross = new Sprite("Sprites\\cross0.png", _CBSettings.spriteName + " Centre");
	}

	//The actual locations of the collision box points on the window are calculated
	for (int i = 0; i < 4; i++) {
		_boxCornerLocations[i].x =  _CBSettings.locationPercent.x + _boxCornersPercent[i].x;
		_boxCornerLocations[i].y =  _CBSettings.locationPercent.y + _boxCornersPercent[i].y;
		_boxCornerLocations[i].colliding = false;
	}	
	updateCollisionBox(spriteSettings);
}

void CollisionBox::UpdateBoxCornerLocations(SpriteSettings spriteSettings) {
	PointXY centre;
	centre.x = 50;
	centre.y = 50;
	//Keeps the collision box's position relative to the sprite it is linked to
	for (int i = 0; i < 4; i++) {
		_boxCornerLocations[i].x += (spriteSettings.locationPercent.x - _CBSettings.locationPercent.x);
		_boxCornerLocations[i].y += (spriteSettings.locationPercent.y - _CBSettings.locationPercent.y);
		_boxCornerLocations[i].colliding = false;
	}
}


void CollisionBox::renderCollisionBox() {
	//A debug procedure that shows the collision box on the screen
	_CBSettings.opacity = 0.5;
	for (int i = 0; i < 4; i++) {
		_cross[i]->setScalePercent(1);
		_cross[i]->getSpriteSettings().locationPercent.x = _boxCornerLocations[i].x - (_cross[i]->getSpriteSettings().spriteDimensionsPercent.sizeX / 2);
		_cross[i]->getSpriteSettings().locationPercent.y = _boxCornerLocations[i].y - (_cross[i]->getSpriteSettings().spriteDimensionsPercent.sizeY / 2);
		_cross[i]->Render();
		_movedCross[i]->setScalePercent(1);
		_movedCross[i]->getSpriteSettings().locationPercent.x = _rotatedBoxLocations[i].x - (_movedCross[i]->getSpriteSettings().spriteDimensionsPercent.sizeX / 2);
		_movedCross[i]->getSpriteSettings().locationPercent.y = _rotatedBoxLocations[i].y - (_movedCross[i]->getSpriteSettings().spriteDimensionsPercent.sizeY / 2);
		_movedCross[i]->Render();	
	}
	_centreCross->setScalePercent(1);
	_centreCross->getSpriteSettings().locationPercent.x = CalculateCentreOfBox().x - (_centreCross->getSpriteSettings().spriteDimensionsPercent.sizeX / 2);
	_centreCross->getSpriteSettings().locationPercent.y = CalculateCentreOfBox().y - (_centreCross->getSpriteSettings().spriteDimensionsPercent.sizeY / 2);
	_centreCross->Render();
}

void CollisionBox::updateCollisionBox(SpriteSettings spriteSettings) {
	//Updates all necessary settings of the collision box to track it's parent sprite
	UpdateBoxCornerLocations(spriteSettings);
	_CBSettings.locationPercent = spriteSettings.locationPercent;
	_CBSettings.spriteDimensionsPercent = spriteSettings.spriteDimensionsPercent;
	_CBSettings.scalePercent = spriteSettings.scalePercent;
}

PointXY CollisionBox::CalculateCentreOfBox() {
	//Calculates the centre of the box by finding the middle point between opposing points
	PointXY CentreOfBox;
	CentreOfBox.x = _boxCornerLocations[0].x + (_boxCornerLocations[3].x - _boxCornerLocations[0].x) / 2;
	CentreOfBox.y = _boxCornerLocations[0].y + (_boxCornerLocations[3].y - _boxCornerLocations[0].y) / 2;
	return CentreOfBox;
}
