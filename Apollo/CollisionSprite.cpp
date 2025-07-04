#include "CollisionSprite.h"

queue<CollisionSprite*> CollisionSprite::_collidingCollisionSprites;

CollisionSprite::CollisionSprite(string filename, string spriteName) : Sprite(filename, spriteName) {
}

CollisionSprite::~CollisionSprite(){
}


//Initialises a rectangular collision box over the sprite
void CollisionSprite::createRectangularCollisionBox() {
	_spriteCollisionBox = new CollisionBox(_sSettings.spriteName);
	_spriteCollisionBox->setupRectangularBox(_sSettings);
}

void CollisionSprite::Update() {
	_spriteCollisionBox->updateCollisionBox(_sSettings);
}

void CollisionSprite::Rotate(float angle, PointXY centrePoint) {
	collisionBoxPoint oldPoint;
	collisionBoxPoint newPoint;
	_sSettings.angleFV += angle;
	//Adjusts the angle so that it remains between 0 and 359
	if (_sSettings.angleFV >= 360) _sSettings.angleFV -= 360;
	else if (_sSettings.angleFV < 0) _sSettings.angleFV += 360;
	_sSettings.rotationMatrix = D2D1::Matrix3x2F::Rotation(_sSettings.angleFV, D2D1::Point2F(centrePoint.x, centrePoint.y));

	for (int cornerNo = 0; cornerNo < 4; cornerNo++) {
		//Calculates the location of the on the screen in pixels due to y axis and x axis in percent not being a 1:1 ratio
		oldPoint.x = ((_spriteCollisionBox->getBoxCornerLocations()[cornerNo].x / 100) * _gfx->getWindowSize().sizeX);
		oldPoint.y = ((_spriteCollisionBox->getBoxCornerLocations()[cornerNo].y / 100) * _gfx->getWindowSize().sizeY);

		newPoint = getRotatedPoint(oldPoint, angle, centrePoint);

		//Points are converted back to percent
		newPoint.x = (newPoint.x / _gfx->getWindowSize().sizeX) * 100;
		newPoint.y = (newPoint.y / _gfx->getWindowSize().sizeY) * 100;

		_spriteCollisionBox->getBoxCornerLocations()[cornerNo] = newPoint;
	}
}

collisionBoxPoint CollisionSprite::getRotatedPoint(collisionBoxPoint oldPoint, float angle, PointXY centrePoint) {
	collisionBoxPoint newPoint;

	//Converts the specified angle in degrees to radians
	float radianAngle = angle * float(pi / 180);

	//Points are rotated using the equation:
	//x' = x * cos(angle) - y * sin(angle)
	//y' = x * sin(angle) + y * cos(angle)
	newPoint.x = ((oldPoint.x - centrePoint.x) * cos(radianAngle)) - ((oldPoint.y - centrePoint.y) * sin(radianAngle)) + centrePoint.x;
	newPoint.y = ((oldPoint.x - centrePoint.x) * sin(radianAngle)) + ((oldPoint.y - centrePoint.y) * cos(radianAngle)) + centrePoint.y;
	return newPoint;
}
