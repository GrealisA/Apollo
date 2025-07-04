#pragma once
#include "CollisionSprite.h"
#include "Explosion.h"
#include "Flames.h"

struct stageSettings {
	int StageNo;
	float FuelMass, FuelMaxCapacity, MaxPropellingForce, FuelBurnRate;
	PointXY CentreOfRotation;
	vector<PointXY> flameLocations;
};

class RocketStage : public CollisionSprite {
public:
	RocketStage(int rocketNo, int stageNo, vector<PointXY> flameLocations, float mass, float fuelMaxCapacity, float maxPropellingForce, float fuelBurnRate);
	~RocketStage();

	void ExplodeStage();
	float getCorrectedAltitude();
	void Render();
	bool& getExploded() { return _exploded; }
	stageSettings& getStageProperties() { return _stageProperties; }
	vector<Flames*> getRocketFlames() { return _rocketFlames; }
private:
	bool _exploded = false;
	stageSettings _stageProperties;
	Explosion* _explosion;
	vector<Flames*> _rocketFlames;
};

