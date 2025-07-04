#include "RocketSaveState.h"

RocketSaveState::RocketSaveState(){
}


RocketSaveState::~RocketSaveState()
{
}

void RocketSaveState::Save(Rocket* rocket) {
	_rocketSettings = rocket->getRocketSettings();
	_rocketPhysics = rocket->getRocketPhysics();

	//Stores all data about the rocket stages at the moment of saving
	for (int stageNo = 0; stageNo < rocket->getRocketStages().size(); stageNo++) {
		_stageExploded.push_back(rocket->getRocketStages()[stageNo]->getExploded());
		_stagePhysics.push_back(rocket->getRocketStages()[stageNo]->getSpritePhysics());
		_stageSettings.push_back(rocket->getRocketStages()[stageNo]->getSpriteSettings());
		_stageProperties.push_back(rocket->getRocketStages()[stageNo]->getStageProperties());
	}

	//Stores all data about the detached rocket stages at the moment of saving
	for (int stageNo = 0; stageNo < rocket->getDetachedStages().size(); stageNo++) {
		_detachedStageExploded.push_back(rocket->getDetachedStages()[stageNo]->getExploded());
		_detachedStagePhysics.push_back(rocket->getDetachedStages()[stageNo]->getSpritePhysics());
		_detachedStageSettings.push_back(rocket->getDetachedStages()[stageNo]->getSpriteSettings());
		_detachedStageProperties.push_back(rocket->getDetachedStages()[stageNo]->getStageProperties());
	}

	_currentStageNo = rocket->getCurrentRocketStage().getStageProperties().StageNo;
	_noDetachedStages = int(rocket->getDetachedStages().size());
	_rocketEngineOn = float(rocket->getRocketEngineOn());
	_exploded = rocket->getExploded();
}


void RocketSaveState::RetrieveSave(Rocket& rocket) {
	rocket.getRocketSettings() = _rocketSettings;
	rocket.getRocketPhysics() = _rocketPhysics;

	//Retrieves all data about the rocket stages from the last save
	for (int stageNo = 0; stageNo < _rocketStages.size(); stageNo++) {
		rocket.getRocketStages()[stageNo]->getExploded() = _stageExploded[stageNo];
		rocket.getRocketStages()[stageNo]->getSpritePhysics() = _stagePhysics[stageNo];
		rocket.getRocketStages()[stageNo]->getSpriteSettings() = _stageSettings[stageNo];
		rocket.getRocketStages()[stageNo]->getStageProperties() = _stageProperties[stageNo];
	}

	//Retrieves all data about the detached rocket stages from the last save
	for (int stageNo = 0; stageNo < _noDetachedStages; stageNo++) {
		rocket.DetachStage();
		rocket.getDetachedStages()[stageNo]->getExploded() = _detachedStageExploded[stageNo];
		rocket.getDetachedStages()[stageNo]->getSpritePhysics() = _detachedStagePhysics[stageNo];
		rocket.getDetachedStages()[stageNo]->getSpriteSettings() = _detachedStageSettings[stageNo];
		rocket.getDetachedStages()[stageNo]->getStageProperties() = _detachedStageProperties[stageNo];
	}

	rocket.getCurrentRocketStage() = *(rocket.getRocketStages()[_currentStageNo]);

	rocket.getRocketEngineOn() = _rocketEngineOn;
	rocket.getExploded() = _exploded ;
}