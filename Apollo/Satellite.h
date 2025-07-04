#pragma once
#include "CollisionSprite.h"

class Satellite : public CollisionSprite {
public:
	Satellite(string satelliteName, int finishingAltitude, float scaleMin, int scaleRange);
	~Satellite();
	void GenerateRandomLocation(VectorQuantity rocketDisplacement);
	void UpdateSatellite(PhysicsVector rocketPhysics, SpriteSettings rocketSettings, int frameRate);

private:
	int _maxLevelAltitude;
	float _minimumScale;
	int _scaleRange;
};

