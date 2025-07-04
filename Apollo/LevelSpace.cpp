#include "LevelSpace.h"

void LevelSpace::Load(Graphics* gfx, FrameRateHandler* frHandler) {
	_gfx = gfx;
	_frHandler = frHandler;

	//Creates a save point for the rocket at the start of the level
	_rocketRespawnState.Save(_rocket);

	_sputnik = new Satellite("Sputnik", maxAltitudeFromGround - 97000, 20, 30);
	_sputnik->GenerateRandomLocation(_rocket->getRocketPhysics().displacement);

	//Initialises the sizes and locations of each sprite
	_starsBackground.push_front(createSprite("LevelSpace", "Stars"));
	_starsBackground.front()->setScalePercent(150);
	_starsBackground.front()->getSpriteSettings().locationPercent.x = (100 - _starsBackground.front()->getSpriteSettings().spriteDimensionsPercent.sizeX) / 2;
	_starsBackground.front()->getSpriteSettings().locationPercent.y = 100 - _starsBackground.front()->getSpriteSettings().spriteDimensionsPercent.sizeY;
	generateStars();

	_earthBackground = createSprite("LevelSpace", "Earth");
	_earthBackground->setScalePercent(150);
	_earthBackground->getSpriteSettings().locationPercent.x = (100 - _earthBackground->getSpriteSettings().spriteDimensionsPercent.sizeX) / 2;
	_earthBackground->getSpriteSettings().locationPercent.y = 125 - _earthBackground->getSpriteSettings().spriteDimensionsPercent.sizeY;

	_moonBackground = createSprite("LevelSpace", "Moon");
	_moonBackground->setScalePercent(150);
	_moonBackground->getSpriteSettings().locationPercent.x = (100 - _moonBackground->getSpriteSettings().spriteDimensionsPercent.sizeX) / 2;
	_moonBackground->getSpriteSettings().locationPercent.y = -330;

	//Initialisation of the HUD
	_hud = new HUD(_rocket, _gfx, 1);

	_GFS = calculateEarthGFS() - calculateMoonGFS();

	//Initialises the background music for the level
	_backgroundMusic = new Sound("\\Sound\\SpaceBackground.wav", -1000, true);
}

void LevelSpace::Unload() {
	_backgroundMusic->Stop();
	_hud->~HUD();
}

void LevelSpace::generateStars() {
	//Checks if the top star sprite is about to reach the top of the screen and creates a new one above it to create a continuous stream of stars
	if (_starsBackground.front()->getSpriteSettings().locationPercent.y >= -50) {
		_starsBackground.push_front(new Sprite(*_starsBackground.front()));
		_starsBackground.front()->getSpriteSettings().locationPercent.y -= _starsBackground.front()->getSpriteSettings().spriteDimensionsPercent.sizeY;
	}
	//Checks if the bottom star sprite is about to reach the bottom of the screen and creates a new one below it to create a continuous stream of stars 
	if (_starsBackground.back()->getSpriteSettings().locationPercent.y + _starsBackground.back()->getSpriteSettings().spriteDimensionsPercent.sizeY >= 150) {
		_starsBackground.push_back(new Sprite(*_starsBackground.back()));
		_starsBackground.back()->getSpriteSettings().locationPercent.y -= _starsBackground.back()->getSpriteSettings().spriteDimensionsPercent.sizeY;
	}
}

void LevelSpace::Update(float totalTime, float deltaTime) {
	UpdateFrame();
	_GFS = calculateEarthGFS() - calculateMoonGFS();
	_rocket->UpdateFuelMass();
	generateStars();
	_spaceInQueue = false;

	//Checks for any key presses that will alter the rocket
	while (!_keyPressed.empty()) {
		_rocket->CheckRocketInput(_keyPressed, 0.01f, true, false);

		//Returns the user to the main menu when esc pressed
		if (_keyPressed.front() == "Escape") setLevelNo(0);
		//Causes the rocket to take off or detaches a stage if it has already launched
		else if (_keyPressed.front() == "Space") {
			if (!_spacePressed) {
				if (_rocket->getCurrentRocketStage().getStageProperties().StageNo != 0) {
					_hud->RemoveCurrentStageHUD();
					_rocket->DetachStage();
				}
				_spacePressed = true;
			}   
			_spaceInQueue = true;
		}
		_keyPressed.pop();
	}
	if (!_spaceInQueue) _spacePressed = false;

	//Limits the propelling force to be between 0 and the specified maximum force of the stage
	if (_rocket->getRocketPhysics().force.general < 0)
		_rocket->getRocketPhysics().force.general = 0;
	if (_rocket->getRocketPhysics().force.general > _rocket->getCurrentRocketStage().getStageProperties().MaxPropellingForce) 
		_rocket->getRocketPhysics().force.general = float(_rocket->getCurrentRocketStage().getStageProperties().MaxPropellingForce);

	if (!_rocket->getExploded()) {
		_rocket->PhysicsCalculations(_rocket->getRocketEngineOn(), _GFS);
		truncateDecimalPlaces(_rocket->getRocketPhysics().velocity.vertical, 4);
		truncateDecimalPlaces(_rocket->getRocketPhysics().velocity.horizontal, 4);

		for (int stageNo = 0; stageNo < _rocket->getRocketStages().size(); stageNo++) {
			_rocket->getRocketStages()[stageNo]->getSpriteSettings().rotatedLocationPercent.x = _rocket->getRocketStages()[stageNo]->getSpriteSettings().locationPercent.x;
			_rocket->getRocketStages()[stageNo]->getSpriteSettings().rotatedLocationPercent.y = _rocket->getRocketStages()[stageNo]->getSpriteSettings().locationPercent.y;

			//Updates the rocket stages' displacement so that it is accurate with the distance the rocket has travelled
			_rocket->getRocketStages()[stageNo]->getSpritePhysics().displacement.vertical += _rocket->getRocketPhysics().velocity.vertical / _frHandler->getFrameRate();
			_rocket->getRocketStages()[stageNo]->getSpritePhysics().displacement.horizontal -= _rocket->getRocketPhysics().velocity.horizontal / _frHandler->getFrameRate();
		}

		//Moves background sprites at a rate which emulates their distance from the rocket
		_sputnik->UpdateSatellite(_rocket->getRocketPhysics(), _rocket->getRocketSettings(), _frHandler->getFrameRate());
		_earthBackground->getSpriteSettings().locationPercent.x -= _rocket->getRocketPhysics().velocity.horizontal / (_frHandler->getFrameRate() * 500.0f);
		_earthBackground->getSpriteSettings().locationPercent.y += _rocket->getRocketPhysics().velocity.vertical / (_frHandler->getFrameRate() * 1000.0f);
		_moonBackground->getSpriteSettings().locationPercent.x -= _rocket->getRocketPhysics().velocity.horizontal / (_frHandler->getFrameRate() * 500.0f);
		_moonBackground->getSpriteSettings().locationPercent.y += _rocket->getRocketPhysics().velocity.vertical / (_frHandler->getFrameRate() * 1000.0f);

		for (int starNo = 0; starNo < _starsBackground.size(); starNo++) {
			_starsBackground[starNo]->getSpriteSettings().locationPercent.x -= _rocket->getRocketPhysics().velocity.horizontal / (_frHandler->getFrameRate() * 500.0f);
			_starsBackground[starNo]->getSpriteSettings().locationPercent.y += _rocket->getRocketPhysics().velocity.vertical / (_frHandler->getFrameRate() * 1000.0f);
		}

		//Updates the rocket's displacement so that it is accurate with the distance the rocket has travelled
		_rocket->getRocketPhysics().displacement.vertical += _rocket->getRocketPhysics().velocity.vertical / _frHandler->getFrameRate();
		_rocket->getRocketPhysics().displacement.horizontal -= _rocket->getRocketPhysics().velocity.horizontal / _frHandler->getFrameRate();

		//Slows down the rocket's rotation so that it doesn't remain in constant motion
		_rocket->getRocketPhysics().angularVelocity -= _rocket->getRocketPhysics().angularVelocity / 1000;
		_rocket->Rotate(_rocket->getRocketPhysics().angularVelocity, _gfx->getCentreOfScreen());

		//Checks for collisions between Sputnik and the rocket stages
		for each(CollisionSprite* rocketStage in _rocket->getRocketStages())
			checkCollisions(rocketStage, _sputnik);

		while (!CollisionSprite::getCollidingCollisionSprites().empty()) {
			CollisionSprite::getCollidingCollisionSprites().pop(); CollisionSprite::getCollidingCollisionSprites().pop();	

			//When the rocket and sputnik collide, the fuel mass is increased and a new location is generated for the satellite
			_rocket->getCurrentRocketStage().getStageProperties().FuelMass += 500000;
			if (_rocket->getCurrentRocketStage().getStageProperties().FuelMass > _rocket->getCurrentRocketStage().getStageProperties().FuelMaxCapacity)
				_rocket->getCurrentRocketStage().getStageProperties().FuelMass = _rocket->getCurrentRocketStage().getStageProperties().FuelMaxCapacity;
			_sputnik->GenerateRandomLocation(_rocket->getRocketPhysics().displacement);
		}
	}
	else {
		//Blows up any stages that haven't already exploded
		for each(RocketStage* stage in _rocket->getRocketStages()) 
			if (!stage->getExploded()) stage->ExplodeStage();
		_respawnTimer++;
	}

	for (int detachedStageNo = 0; detachedStageNo < _rocket->getDetachedStages().size(); detachedStageNo++) {
		//Allows gravity to affect the detached stage until it explodes
		if (!_rocket->getDetachedStages()[detachedStageNo]->getExploded())
			_rocket->getDetachedStages()[detachedStageNo]->getSpritePhysics().velocity.vertical -= _GFS / _frHandler->getFrameRate();
		else
			_rocket->getDetachedStages()[detachedStageNo]->getSpritePhysics().velocity.vertical = 0;

		//Updates the location of the selected detached stage
		_rocket->getDetachedStages()[detachedStageNo]->getSpritePhysics().displacement.horizontal += (_rocket->getDetachedStages()[detachedStageNo]->getSpritePhysics().velocity.horizontal + _rocket->getRocketPhysics().velocity.horizontal) / _frHandler->getFrameRate();
		_rocket->getDetachedStages()[detachedStageNo]->getSpritePhysics().displacement.vertical -= (_rocket->getDetachedStages()[detachedStageNo]->getSpritePhysics().velocity.vertical - _rocket->getRocketPhysics().velocity.vertical) / _frHandler->getFrameRate();
		_rocket->getDetachedStages()[detachedStageNo]->getSpriteSettings().locationPercent.x += (_rocket->getDetachedStages()[detachedStageNo]->getSpritePhysics().velocity.horizontal + _rocket->getRocketPhysics().velocity.horizontal) / _frHandler->getFrameRate();
		_rocket->getDetachedStages()[detachedStageNo]->getSpriteSettings().locationPercent.y -= (_rocket->getDetachedStages()[detachedStageNo]->getSpritePhysics().velocity.vertical - _rocket->getRocketPhysics().velocity.vertical) / _frHandler->getFrameRate();
		_rocket->CalculateStageRotatedLocations(_rocket->getDetachedStages()[detachedStageNo], _gfx->getCentreOfScreen());
	}

	if (_rocket->getRocketEngineOn()) _rocket->UpdateFlames();
	//If the satellite goes out of range vertically, a new satellite is generated ahead of the rocket 
	if (_sputnik->getSpriteSettings().locationPercent.y > 20000) _sputnik->GenerateRandomLocation(_rocket->getRocketPhysics().displacement);

	//Updates the collision boxes
	for each (RocketStage* stage in _rocket->getRocketStages()) stage->Update();
	for each (RocketStage* detachedStage in _rocket->getDetachedStages()) detachedStage->Update();
	_hud->Update(_rocket, _sputnik);

	//Switches to the moon level when the player reaches the specified altitude
	if (_rocket->getRocketPhysics().displacement.vertical > maxAltitudeFromGround - 97000)
		setLevelNo(5);
	//Respawns the player after 150 frames have occurred after an explosion
	else if (_rocket->getExploded() && _respawnTimer > 150) {
		_rocket = new Rocket(0, float(karmanLine), _gfx, _frHandler);
		_rocketRespawnState.RetrieveSave(*_rocket);
		_respawnTimer = 0;
		setLevelNo(4);
	}
	//Causes the rocket to explode when the player attempts to re-enter the earth atmosphere
	else if (_rocket->getRocketPhysics().displacement.vertical < karmanLine)
		_rocket->getExploded() = true;
}

void LevelSpace::Render(float totalTime) {
	//Background Rendering
	D2D1::ColorF* skyColour = new D2D1::ColorF(0, 0, 0, 1);
	_gfx->ClearScreen(*skyColour);
	for each(Sprite* star in _starsBackground) star->Render();
	_earthBackground->Render();
	_moonBackground->Render();

	//Foreground Rendering	
	_rocket->Render(_frame);
	_sputnik->Render();

	//Collision Box Rendering for debug purposes
	if (collisionBoxMode)
		for each(CollisionSprite* rocketStage in _rocket->getRocketStages()) rocketStage->getSpriteCollisionBox()->renderCollisionBox();
	
	//HUD rendering
	_hud->Render(_frame);
}