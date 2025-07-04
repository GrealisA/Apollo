#include "Rocket.h"

Rocket::Rocket(int rocketNo, float startingAltitude, Graphics* gfx, FrameRateHandler* frHandler) {
	float currentStageYPosition = 0;

	_frHandler = frHandler;
	_gfx = gfx;
	_rocketNo = rocketNo;
	_rocketSettings.rotationMatrix = D2D1::Matrix3x2F::Identity();

	//Initialises the rocket depending on which rocket is selected. This program only uses one however can incorporate more if future development occurs
	if (rocketNo == 0) {
		_rocketStages.resize(3);
		
		_rocketSettings.spriteDimensionsPercent.sizeX = _sizeOfRocket;
		_rocketSettings.spriteDimensionsPercent.sizeY = (_rocketSettings.spriteDimensionsPercent.sizeX * _gfx->getWindowSize().sizeX * 6) / _gfx->getWindowSize().sizeY;

		_rocketSettings.locationPercent.x = 50 - (_rocketSettings.spriteDimensionsPercent.sizeX/2);
		_rocketSettings.locationPercent.y = 50 - (_rocketSettings.spriteDimensionsPercent.sizeY/2);
		_rocketPhysics.displacement.vertical = startingAltitude;
		for (int stageNo = 0; stageNo <= 2; stageNo++) {
			float mass, fuelMaxCapacity, maxPropellingForce, fuelBurnRate;
			vector<PointXY> flameLocations;
			float scalePercent = _sizeOfRocket;
			float rocketStageEngineDistance = 0;
			float centreOfRotationScale;

			//Allocates the properties of each individual stage
			switch (stageNo) {
			case 0:
				mass = 13500;
				fuelMaxCapacity = 109500;
				fuelBurnRate = fuelMaxCapacity / 500;
				maxPropellingForce = 1000000;
				scalePercent /= 2.5f;
				flameLocations.resize(1);
				flameLocations[0].x = 0.5f * scalePercent;
				centreOfRotationScale = 0.5f;
				break;
			case 1:
				mass = 40100;
				fuelMaxCapacity = 465100;
				fuelBurnRate = fuelMaxCapacity / 180;
				maxPropellingForce = 5141000;
				scalePercent /= 1.2f;
				flameLocations.resize(2);
				flameLocations[0].x = 0.4f * scalePercent;
				flameLocations[1].x = 0.6f * scalePercent;
				rocketStageEngineDistance = _rocketStages[stageNo - 1]->getSpriteSettings().spriteDimensionsPercent.sizeY * 0.09f;
				centreOfRotationScale = 0.6f;
				break;
			case 2: 
				mass = 130000;
				fuelMaxCapacity = 500000;
				fuelBurnRate = fuelMaxCapacity / 168;
				maxPropellingForce = 40000000;
				flameLocations.resize(3);				
				flameLocations[0].x = 0.5f * scalePercent;
				flameLocations[1].x = 0.15f * scalePercent;
				flameLocations[2].x = 0.85f * scalePercent;
				rocketStageEngineDistance = _rocketStages[stageNo - 1]->getSpriteSettings().spriteDimensionsPercent.sizeY * 0.32f;
				centreOfRotationScale = 0.55f;
				break;
			}
			//Initialisation of the current rocket stage
			_rocketStages[stageNo] = new RocketStage(rocketNo, stageNo, flameLocations, mass, fuelMaxCapacity, maxPropellingForce, fuelBurnRate);
			_rocketStages[stageNo]->setScalePercent(scalePercent);
			for (int flameNo = 0; flameNo < flameLocations.size(); flameNo++)
				_rocketStages[stageNo]->getStageProperties().flameLocations[flameNo].y = _rocketStages[stageNo]->getSpriteSettings().spriteDimensionsPercent.sizeY;
			
			//Stacks each individual stage on top of each stages engine space
			_rocketStages[stageNo]->getSpriteSettings().locationPercent.x = 50 - (_rocketStages[stageNo]->getSpriteSettings().spriteDimensionsPercent.sizeX/2);
			_rocketStages[stageNo]->getSpriteSettings().locationPercent.y = _rocketSettings.locationPercent.y + currentStageYPosition - rocketStageEngineDistance;

			currentStageYPosition += _rocketStages[stageNo]->getSpriteSettings().spriteDimensionsPercent.sizeY;			
			_rocketStages[stageNo]->getStageProperties().CentreOfRotation.x = _rocketStages[stageNo]->getSpriteSettings().spriteDimensionsPercent.sizeX / 2;
			_rocketStages[stageNo]->getStageProperties().CentreOfRotation.y = currentStageYPosition * centreOfRotationScale;
		}
		float changeInCentre = (_rocketStages[2]->getStageProperties().CentreOfRotation.y + _rocketSettings.locationPercent.y - 50);
		_rocketSettings.locationPercent.y -= changeInCentre;
		
		for each(RocketStage* stage in _rocketStages) {
			stage->getSpriteSettings().locationPercent.y -= changeInCentre;		
			//Overall mass of the rocket is calculated 
			_rocketPhysics.mass += stage->getSpritePhysics().mass;
			_rocketPhysics.mass += stage->getStageProperties().FuelMass;
		}
	}

	for (int stageNo = 0; stageNo < _rocketStages.size(); stageNo++) {
		_rocketStages[stageNo]->createRectangularCollisionBox();	
		_rocketStages[stageNo]->getSpritePhysics().displacement.vertical = CalculateInitialStageAltitudes(stageNo, startingAltitude);
	}

	//Starting location of the rocket is initialised
	_rocketStartingLocation.x = 0;
	_rocketStartingLocation.y = _rocketStages[_rocketStages.size() - 1]->getSpriteSettings().locationPercent.y + _rocketStages[_rocketStages.size() - 1]->getSpriteSettings().spriteDimensionsPercent.sizeY;

	_rocketEngineOn = false;
	_exploded = false;
	_currentStage = _rocketStages.back();
	UpdateFlames();
}
Rocket::~Rocket() { 
	for each(RocketStage* stage in _rocketStages) stage->~RocketStage();
}

void Rocket::CheckRocketInput(queue<string> keyPressed, float rotationSpeed, bool activeSpeedBrake, bool crashed) {
	//Rotates the orientation of the rocket anticlockwise by adjusting the rocket's angular velocity
	if (keyPressed.front() == "Left") {
		if (!crashed && _rocketEngineOn && _rocketPhysics.velocity.vertical != 0)
			_rocketPhysics.angularVelocity -= rotationSpeed;
	}
	//Rotates the orientation of the rocket clockwise by adjusting the rocket's angular velocity
	else if (keyPressed.front() == "Right") {
		if (!crashed && _rocketEngineOn && _rocketPhysics.velocity.vertical != 0)
			_rocketPhysics.angularVelocity += rotationSpeed;
	}
	//Increases the throttle of the rocket until at the maximum throttle available
	else if (keyPressed.front() == "Up") {
		if (_rocketEngineOn) {
			if (_rocketPhysics.force.general <= _currentStage->getStageProperties().MaxPropellingForce)
				_rocketPhysics.force.general += _currentStage->getStageProperties().MaxPropellingForce / 80.0f;
			else
				_rocketPhysics.force.general = float(_currentStage->getStageProperties().MaxPropellingForce);
		}
		Rotate(0, _gfx->getCentreOfScreen());
	}
	//Decreases the throttle of the rocket until at the minimum throttle available
	else if (keyPressed.front() == "Down") {
		if (_rocketEngineOn) {
			if (_rocketPhysics.force.general >= 0.5f)
				_rocketPhysics.force.general -= _currentStage->getStageProperties().MaxPropellingForce / 40.0f;
			else {
				_rocketPhysics.force.general = 0.5f;
			}
		}
		Rotate(0, _gfx->getCentreOfScreen());
	}
	//Enables the speed brakes of the rocket
	else if (activeSpeedBrake && keyPressed.front() == "Control") {
		_rocketPhysics.velocity.horizontal *= 0.95f;
		_rocketPhysics.velocity.vertical *= 0.95f;
	}
}

void Rocket::DetachStage() {
	if (_currentStage->getStageProperties().StageNo > 0) {
		_currentStage->getStageProperties().FuelMass = 0;

		//Removes the current stage from the rocket and identifies it as a detached stage while updating the rocket to match the settings of the new current stage
		_detachedRocketStages.push_back(new RocketStage(*_currentStage));
		_detachedRocketStages.back()->getSpritePhysics().velocity.vertical *= 0.98f;
		CalculateStageRotatedLocations(_detachedRocketStages.back(), _gfx->getCentreOfScreen());
		_detachedRocketStages.back()->Update();
		CentraliseCentreOfMass();
		_rocketStages.pop_back();
		_currentStage = _rocketStages.back();
		for each(Flames* rocketFlame in _currentStage->getRocketFlames())
			rocketFlame->Rotate(_currentStage->getSpriteSettings().angleFV, _gfx->getCentreOfScreen());
	}
}

void Rocket::CentraliseCentreOfMass() {
		float centreOfScreenDifference = _currentStage->getStageProperties().CentreOfRotation.y - _rocketStages[_currentStage->getStageProperties().StageNo - 1]->getStageProperties().CentreOfRotation.y;

		//Moves all rocket sprites to a location that is correct with the current centre of mass when it is in the centre of the screen
		for each(RocketStage* stage in _rocketStages) {
			stage->getSpriteSettings().locationPercent.y += centreOfScreenDifference;
			stage->Update();
		}
		for each(RocketStage* detachedStage in _detachedRocketStages)
			detachedStage->getSpriteSettings().locationPercent.y += centreOfScreenDifference;
		for each(Flames* rocketFlame in _currentStage->getRocketFlames())
			rocketFlame->getSpriteSettings().locationPercent.y += centreOfScreenDifference;
}

void Rocket::PhysicsCalculations(bool rocketOn, float GFS) {
	if (rocketOn) {
		//Rocket force calculations
		_rocketPhysics.force.horizontal = _rocketSettings.rotationMatrix._12 * _rocketPhysics.force.general;
		_rocketPhysics.force.vertical = _rocketSettings.rotationMatrix._22 * _rocketPhysics.force.general;

		//Rocket acceleration calculations
		_rocketPhysics.acceleration.vertical = (_rocketPhysics.force.vertical - (_rocketPhysics.mass * GFS)) / _rocketPhysics.mass;
		_rocketPhysics.acceleration.horizontal = (_rocketPhysics.force.horizontal) / _rocketPhysics.mass;

		//Rocket velocity calculations
		_rocketPhysics.velocity.vertical += _rocketPhysics.acceleration.vertical / _frHandler->getFrameRate();
		_rocketPhysics.velocity.horizontal += _rocketPhysics.acceleration.horizontal / _frHandler->getFrameRate();
		_rocketPhysics.velocity.general = sqrt(pow(_rocketPhysics.velocity.horizontal, 2) + pow(_rocketPhysics.velocity.vertical, 2));

		//Allocation of current rocket physics to all stages
		for (int stageNo = 0; stageNo < _rocketStages.size(); stageNo++) {
			_rocketStages[stageNo]->getSpritePhysics().acceleration.vertical = _rocketPhysics.acceleration.vertical;
			_rocketStages[stageNo]->getSpritePhysics().acceleration.horizontal = _rocketPhysics.acceleration.horizontal;
			_rocketStages[stageNo]->getSpritePhysics().velocity.horizontal = _rocketPhysics.velocity.horizontal;
			_rocketStages[stageNo]->getSpritePhysics().velocity.vertical = _rocketPhysics.velocity.vertical;
			_rocketStages[stageNo]->getSpriteSettings().rotationMatrix = _rocketSettings.rotationMatrix;
		}
	}
}

void Rocket::UpdateFlames() {
	//Scaling of rocket flames to match the amount of force being output by the engines
	for each(Flames* flames in _currentStage->getRocketFlames())
		flames->setScalePercent(_currentStage->getSpriteSettings().spriteDimensionsPercent.sizeX * (_rocketPhysics.force.general / _currentStage->getStageProperties().MaxPropellingForce));

	//Positioning of rocket flames to match the positions of the rocket engines
	if (_rocketNo == 0) {
		for (int flameNo = 0; flameNo < _currentStage->getRocketFlames().size(); flameNo++) {
			_currentStage->getRocketFlames()[flameNo]->getSpriteSettings().locationPercent.x = _currentStage->getSpriteSettings().locationPercent.x + _currentStage->getStageProperties().flameLocations[flameNo].x - (_currentStage->getRocketFlames()[flameNo]->getSpriteSettings().spriteDimensionsPercent.sizeX/2);
			_currentStage->getRocketFlames()[flameNo]->getSpriteSettings().locationPercent.y = _currentStage->getSpriteSettings().locationPercent.y + _currentStage->getStageProperties().flameLocations[flameNo].y;
		}
	}
}

void Rocket::UpdateFuelMass() {
	//Ensures that the fuel cannot decrease below 0
	if (_currentStage->getStageProperties().FuelMass <= 0) {
		_currentStage->getStageProperties().FuelMass = 0;
		_rocketPhysics.force.general = 0;
	}
	else
		_currentStage->getStageProperties().FuelMass -= (_currentStage->getStageProperties().FuelBurnRate * (_rocketPhysics.force.general / (_currentStage->getStageProperties().MaxPropellingForce * 0.83f))) / _frHandler->getFrameRate();
	
	//Resets and remeasures the mass of the rocket from it's stages mass and their fuel mass
	_rocketPhysics.mass = 0;
	for each(RocketStage* stage in _rocketStages) {
		_rocketPhysics.mass += stage->getSpritePhysics().mass;
		_rocketPhysics.mass += stage->getStageProperties().FuelMass;
	}
}

void Rocket::Rotate(float angle, PointXY centrePoint) {
	_rocketSettings.rotationMatrix = _rocketSettings.rotationMatrix * D2D1::Matrix3x2F::Rotation(angle, D2D1::Point2F(centrePoint.x, centrePoint.y));
	_rocketSettings.angleFV += angle;
	//Keeps the angle between 0 and 359
	if (_rocketSettings.angleFV >= 360) _rocketSettings.angleFV -= 360;
	else if (_rocketSettings.angleFV < 0) _rocketSettings.angleFV += 360;

	for (int stageNo = 0; stageNo < _rocketStages.size(); stageNo++) {
		_rocketStages[stageNo]->Rotate(angle, centrePoint);
	}
	//Rotates flames to track the rocket engines
	for (int flameNo = 0; flameNo < _currentStage->getRocketFlames().size(); flameNo++) {
		_currentStage->getRocketFlames()[flameNo]->Rotate(angle, centrePoint);
		_currentStage->getRocketFlames()[flameNo]->getSpriteSettings().rotatedLocationPercent.x = _currentStage->getRocketFlames()[flameNo]->getSpriteSettings().locationPercent.x;
		_currentStage->getRocketFlames()[flameNo]->getSpriteSettings().rotatedLocationPercent.y = _currentStage->getRocketFlames()[flameNo]->getSpriteSettings().locationPercent.y;
		UpdateFlames();
	}
}

void Rocket::Render(int frame) {
	//Only renders the flames when the rocket has lifted off and hasn't exploded
	if (_rocketEngineOn && !_currentStage->getExploded()) {
		for each(Flames* flames in _currentStage->getRocketFlames()) {
			flames->Render(frame);
		}
	}

	//Renders all stages
	for each(RocketStage* stage in _rocketStages)
		stage->Render();
	for each(RocketStage* detachedStage in _detachedRocketStages)
		detachedStage->Render();
}

void Rocket::CalculateStageRotatedLocations(RocketStage* stage, PointXY centreOfRotation) {
	collisionBoxPoint oldLocation;
	oldLocation.x = ((stage->getSpriteCollisionBox()->getBoxCornerLocations()[0].x / 100) * _gfx->getWindowSize().sizeX);
	oldLocation.y = ((stage->getSpriteCollisionBox()->getBoxCornerLocations()[0].y / 100) * _gfx->getWindowSize().sizeY);
	float angle;

	if (stage->getSpriteSettings().angleFV > 180) angle = stage->getSpriteSettings().angleFV - 360;
	else angle = stage->getSpriteSettings().angleFV;

	//Rotates the base collision box point of the stage to be parallel to the vertical
	collisionBoxPoint newLocation = CollisionSprite::getRotatedPoint(oldLocation, -angle, centreOfRotation);

	stage->getSpriteSettings().rotatedLocationPercent.x = (newLocation.x / _gfx->getWindowSize().sizeX) * 100;
	stage->getSpriteSettings().rotatedLocationPercent.y = (newLocation.y / _gfx->getWindowSize().sizeY) * 100;

	for (int i = 0; i < 4; i++) {
		oldLocation.x = ((stage->getSpriteCollisionBox()->getBoxCornerLocations()[i].x / 100) * _gfx->getWindowSize().sizeX);
		oldLocation.y = ((stage->getSpriteCollisionBox()->getBoxCornerLocations()[i].y / 100) * _gfx->getWindowSize().sizeY);

		newLocation = CollisionSprite::getRotatedPoint(oldLocation, -angle, centreOfRotation);

		stage->getSpriteCollisionBox()->getRotatedBoxLocations()[i].x = (newLocation.x / _gfx->getWindowSize().sizeX) * 100;
		stage->getSpriteCollisionBox()->getRotatedBoxLocations()[i].y = (newLocation.y / _gfx->getWindowSize().sizeY) * 100;
	}
}

//Calculates the displacements of each stage from the ground
float Rocket::CalculateInitialStageAltitudes(int stageNo, float rocketAltitude) {
	if (stageNo < _rocketStages.size() - 1)
		return CalculateInitialStageAltitudes(stageNo + 1, rocketAltitude) + (_rocketStages[stageNo + 1]->getSpriteSettings().spriteDimensionsPercent.sizeY);
	else
		if (rocketAltitude < 0) return 0;
		else return rocketAltitude;
}