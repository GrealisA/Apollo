#pragma once
#include "CollisionSprite.h"

using namespace std;

class Cloud
{
public:
	Cloud(string CloudName, int startingAltitude, int finishingAltitude, int noClouds, float scaleMin, float scaleRange);
	~Cloud();
	
	void GenerateClouds(PhysicsVector rocketPhysics, SpriteSettings rocketSettings);
	void UpdateClouds(VectorQuantity rocketVelocity, SpriteSettings rocketSettings, int frameRate);
	void RenderClouds();	
	
	vector<CollisionSprite*> getClouds() { return _clouds; }

private:	
	CollisionSprite* CreateRandomCloud(PhysicsVector rocketPhysics, SpriteSettings rocketSettings);

	CollisionSprite* _templateCloud;
	vector<CollisionSprite*> _clouds;

	int _noClouds;
	int _minLevelAltitude;
	int _maxLevelAltitude;
	float _nextAltitudeScale;
	float _minimumCloudScale;
	float _cloudScaleRange;

	SizeXY _despawnRange;
};

