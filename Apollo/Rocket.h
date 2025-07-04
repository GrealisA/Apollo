#pragma once
#include <math.h>
#include <vector>
#include "RocketStage.h"
#include "FrameRateHandler.h"


class Rocket{
public:
	Rocket(int rocketNo, float startingAltitude, Graphics* gfx, FrameRateHandler* frHandler);
	~Rocket();
	void Rotate(float angle, PointXY centrePoint);
	void Render(int frame);
	void UpdateFlames();
	void UpdateFuelMass();
	void PhysicsCalculations(bool rocketOn, float GFS);
	void CalculateStageRotatedLocations(RocketStage* stage, PointXY centreOfRotation);
	float CalculateInitialStageAltitudes(int stageNo, float rocketDisplacement);
	void DetachStage();
	void CentraliseCentreOfMass();
	void CheckRocketInput(queue<string> keysPressed, float rotationSpeed, bool activeSpeedBrake, bool crashed);

	vector<RocketStage*> getRocketStages() { return _rocketStages; }
	vector<RocketStage*> getDetachedStages() { return _detachedRocketStages; }

	RocketStage& getCurrentRocketStage() { return *_currentStage; }
	PhysicsVector& getRocketPhysics() { return _rocketPhysics; }	
	SpriteSettings& getRocketSettings() { return _rocketSettings; }

	PointXY& getRocketStartingLocation() { return _rocketStartingLocation; }
	bool& getRocketEngineOn() { return _rocketEngineOn; }
	bool& getExploded() { return _exploded; }
private:

	vector<RocketStage*> _rocketStages;
	vector<RocketStage*> _detachedRocketStages;

	RocketStage* _currentStage;
	SpriteSettings _rocketSettings;

	bool _rocketEngineOn;
	bool _exploded;
	int _rocketNo;
	float _sizeOfRocket = 2.5f;
	PointXY _rocketStartingLocation;
	PhysicsVector _rocketPhysics;	
	Graphics* _gfx;
	FrameRateHandler* _frHandler;
};

