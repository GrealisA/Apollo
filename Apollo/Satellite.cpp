#include "Satellite.h"


Satellite::Satellite(string satelliteName, int finishingAltitude, float scaleMin, int scaleRange) : CollisionSprite("Sprites\\Obstacles\\" + satelliteName + ".png", satelliteName) {
	_maxLevelAltitude = finishingAltitude;
	_minimumScale = scaleMin;
	_scaleRange = scaleRange;
}

Satellite::~Satellite()
{
}

void Satellite::GenerateRandomLocation(VectorQuantity rocketDisplacement) {
	srand((unsigned)rocketDisplacement.vertical);
	setScalePercent((rand() % _scaleRange) + _minimumScale);
	createRectangularCollisionBox();

	//Randomly generates the location of the satellite within the defined limits
	_spritePhysics.displacement.horizontal = float(rand() % maxHorizontalDisplacement);
	_spritePhysics.displacement.vertical = (rand() % int(_maxLevelAltitude - rocketDisplacement.vertical)) + rocketDisplacement.vertical + 500;
	_sSettings.locationPercent.x = _spritePhysics.displacement.horizontal - rocketDisplacement.horizontal;
	_sSettings.locationPercent.y = rocketDisplacement.vertical - _spritePhysics.displacement.vertical;
	
	//Randomly generates both velocities of the satellite due to it moving freely in 2d space
	_spritePhysics.velocity.vertical = float(rand() % 4) - 2;
	_spritePhysics.velocity.horizontal = float(rand() % 10) + 2;

	if (int(rand() % 2) == 0)
		_spritePhysics.velocity.horizontal *= -1;
	Update();
}

void Satellite::UpdateSatellite(PhysicsVector rocketPhysics, SpriteSettings rocketSettings, int frameRate) {
	//Moves the satellite so that it remains relatively positioned with the rocket
	_sSettings.locationPercent.x -= (rocketPhysics.velocity.horizontal + _spritePhysics.velocity.horizontal) / frameRate;
	_sSettings.locationPercent.y += (rocketPhysics.velocity.vertical - _spritePhysics.velocity.vertical) / frameRate;
	Update();
}
