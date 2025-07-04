#include "Cloud.h"

Cloud::Cloud(string CloudName, int startingAltitude, int finishingAltitude, int noClouds, float scaleMin, float scaleRange){
	_templateCloud = new CollisionSprite("Sprites\\Obstacles\\" + CloudName + ".png", CloudName);
	_noClouds = noClouds;
	_minimumCloudScale = scaleMin;
	_cloudScaleRange = scaleRange;
	_minLevelAltitude = startingAltitude;
	_maxLevelAltitude = finishingAltitude;
	_nextAltitudeScale = 0.1f;
	_despawnRange.sizeX = 200;
	_despawnRange.sizeY = 500;
}

Cloud::~Cloud(){

}

CollisionSprite* Cloud::CreateRandomCloud(PhysicsVector rocketPhysics, SpriteSettings rocketSettings) {
	srand((unsigned)rocketPhysics.displacement.vertical);
	CollisionSprite* newCloud;

	//New cloud is created and allocated a collisionbox
	newCloud = new CollisionSprite(*_templateCloud);
	newCloud->setScalePercent(float(rand() % int(_cloudScaleRange)) + _minimumCloudScale);
	newCloud->createRectangularCollisionBox();

	//Cloud velocity is randomly calculated
	newCloud->getSpritePhysics().velocity.horizontal = float(rand() % 15) + 2;
	newCloud->getSpritePhysics().velocity.vertical = 0;

	//Cloud is randomly chosen to move left or right and is allocated to an appropiate location
	if (int(rand() % 2) == 0) {
		newCloud->getSpritePhysics().velocity.horizontal *= -1;
		newCloud->getSpriteSettings().locationPercent.x = rocketSettings.locationPercent.x - (rand() % int(_despawnRange.sizeX / 2)) + 75;
	}
	else 
		newCloud->getSpriteSettings().locationPercent.x = rocketSettings.locationPercent.x + (rand() % int(_despawnRange.sizeX / 2)) - 75;

	newCloud->getSpriteSettings().locationPercent.y = rocketSettings.locationPercent.y - float(rand() % int(_despawnRange.sizeY)) - 100;
	newCloud->Update();
	return newCloud;
}

void Cloud::GenerateClouds(PhysicsVector rocketPhysics, SpriteSettings rocketSettings) {
	if (rocketPhysics.displacement.vertical > _minLevelAltitude) {
		//Spawns a new cloud when the next altitude spawning has been reached
		if (rocketPhysics.displacement.vertical - _minLevelAltitude > (_maxLevelAltitude - _minLevelAltitude) * _nextAltitudeScale) {
			_nextAltitudeScale += 0.1f;
			_noClouds += 1;
		}

		while (_clouds.size() < _noClouds)
			_clouds.push_back(CreateRandomCloud(rocketPhysics, rocketSettings));

		//When cloud goes outside of the despawn range, a new cloud is generated within the range
		if (!_clouds.empty()) {
			for (int CloudNo = 0; CloudNo < _clouds.size(); CloudNo++) {
				if (_clouds[CloudNo]->getSpriteSettings().locationPercent.y > 100
					|| _clouds[CloudNo]->getSpriteSettings().locationPercent.x < -_despawnRange.sizeX / 2
					|| _clouds[CloudNo]->getSpriteSettings().locationPercent.x > _despawnRange.sizeX / 2) {
					_clouds[CloudNo] = CreateRandomCloud(rocketPhysics, rocketSettings);
				}
			}
		}
	}
}

void Cloud::UpdateClouds(VectorQuantity rocketVelocity, SpriteSettings rocketSettings, int frameRate) {
	//Each cloud is updated on the screen to be correctly relative to the rocket
	for each(CollisionSprite* Cloud in _clouds){
		Cloud->getSpritePhysics().displacement.horizontal -= Cloud->getSpritePhysics().velocity.horizontal / frameRate;
		Cloud->getSpritePhysics().displacement.vertical += ((rocketVelocity.vertical * 0.4f) + Cloud->getSpritePhysics().velocity.vertical) / frameRate;
		
		Cloud->getSpriteSettings().locationPercent.x -= (rocketVelocity.horizontal + Cloud->getSpritePhysics().velocity.horizontal) / frameRate;
		Cloud->getSpriteSettings().locationPercent.y += ((rocketVelocity.vertical * 0.4f) - Cloud->getSpritePhysics().velocity.vertical) / frameRate;
		Cloud->Update();
	}
}


void Cloud::RenderClouds() {
	for each(CollisionSprite* Cloud in _clouds) {
		Cloud->Render();
		if (collisionBoxMode)
			Cloud->getSpriteCollisionBox()->renderCollisionBox();
	}
}
