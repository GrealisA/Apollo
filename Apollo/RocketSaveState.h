#pragma once
#include "Rocket.h"

class RocketSaveState
{
public:
	RocketSaveState();
	~RocketSaveState();

	void Save(Rocket* rocket);
	void RetrieveSave(Rocket& rocket);

private:
	vector<RocketStage> _rocketStages;
	vector<RocketStage> _detachedRocketStages;

	SpriteSettings _rocketSettings;

	vector<Flames> _stageFlames;
	vector<bool> _stageExploded;
	vector<PhysicsVector> _stagePhysics;
	vector<SpriteSettings> _stageSettings;
	vector<stageSettings> _stageProperties;

	vector<bool> _detachedStageExploded;
	vector<PhysicsVector> _detachedStagePhysics;
	vector<SpriteSettings> _detachedStageSettings;
	vector<stageSettings> _detachedStageProperties;

	int _currentStageNo;
	int _noDetachedStages;
	bool _rocketEngineOn;
	bool _exploded;
	PhysicsVector _rocketPhysics;
};

