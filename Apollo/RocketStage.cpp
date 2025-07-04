#include "RocketStage.h"

RocketStage::RocketStage(int rocketNo, int stageNo, vector<PointXY> flameLocations, float mass, float fuelMaxCapacity, float maxPropellingForce, float fuelBurnRate) : CollisionSprite("Sprites\\Rockets\\Rocket" + to_string(rocketNo) + " Part" + to_string(stageNo) + ".png", "Rocket" + to_string(rocketNo) + " Stage" + to_string(stageNo)){
	//Initialises the properties of the stage
	_spritePhysics.mass = mass;
	_stageProperties.StageNo = stageNo;
	_stageProperties.FuelMaxCapacity = fuelMaxCapacity;
	_stageProperties.FuelMass = _stageProperties.FuelMaxCapacity;
	_stageProperties.MaxPropellingForce = maxPropellingForce;
	_stageProperties.FuelBurnRate = fuelBurnRate;
	_stageProperties.flameLocations = flameLocations;

	for each(PointXY flame in _stageProperties.flameLocations)
		_rocketFlames.push_back(new Flames(0));
}


RocketStage::~RocketStage(){
	CollisionSprite::~CollisionSprite();
	for each(Flames* flame in _rocketFlames) flame->~Flames();
}

float RocketStage::getCorrectedAltitude() {
	int lowestRocketPoint = 0;
	//Finds the lowest point of the rocket stage and finds the distance between that point and the ground
	for (int i = 1; i < 4; i++) {
		if (getSpriteCollisionBox()->getBoxCornerLocations()[i].y > getSpriteCollisionBox()->getBoxCornerLocations()[lowestRocketPoint].y)
			lowestRocketPoint = i;
	}
	return getSpritePhysics().displacement.vertical + (getSpriteCollisionBox()->getBoxCornerLocations()[lowestRocketPoint].y - (_sSettings.spriteDimensionsPercent.sizeY + _sSettings.locationPercent.y));
}

void RocketStage::ExplodeStage() {
	if (!_exploded) {
		_exploded = true;
		//Creates an explosion at the centre of the stage
		_explosion = new Explosion(0, getSpriteCollisionBox()->CalculateCentreOfBox());
	}
}

void RocketStage::Render(){
	if (!_exploded)
		CollisionSprite::Render();
	else
		_explosion->Render();
}