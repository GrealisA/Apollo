#include "LevelMoon.h"

void LevelMoon::Load(Graphics* gfx, FrameRateHandler* frHandler) {
	_gfx = gfx;
	_frHandler = frHandler;

	//Creates a save point for the rocket at the start of the level
	_rocketRespawnState.Save(_rocket);

	_rocket->getRocketPhysics().displacement.vertical = 100000;

	//Initialises the sizes and locations of each sprite
	_ground.push_front(createCollisionSprite("LevelMoon", "GroundMoon", float(_groundScalePercent), 5.972f * float(pow(10, 24))));
	_ground.front()->getSpriteSettings().locationPercent.x = _rocket->getRocketStartingLocation().x - (_groundScalePercent / 2.0f) + 50;
	_ground.front()->getSpriteSettings().locationPercent.y = _rocket->getRocketPhysics().displacement.vertical;
	_ground.front()->getSpriteSettings().spriteDimensionsPercent.sizeY = 1000;
	_ground.front()->createRectangularCollisionBox();
	procedurallyGenerateGround();

	_moonBackground = createSprite("LevelMoon", "Background Moon");
	_moonBackground->setScalePercent(200);
	_moonBackground->getSpriteSettings().locationPercent.x = 50 - (_moonBackground->getSpriteSettings().spriteDimensionsPercent.sizeX / 2.0f);
	_moonBackground->getSpriteSettings().locationPercent.y = 505;

	_earth = createSprite("LevelMoon", "Earth");
	_earth->setScalePercent(30);
	_earth->getSpriteSettings().locationPercent.x = 10;
	_earth->getSpriteSettings().locationPercent.y = 10;

	_stars = createSprite("LevelSpace", "Stars");
	_stars->setScalePercent(120);
	_stars->getSpriteSettings().locationPercent.x = 50 - (_stars->getSpriteSettings().spriteDimensionsPercent.sizeX / 2.0f);
	_stars->getSpriteSettings().locationPercent.y = -10;

	//Initialisation of the HUD
	_hud = new HUD(_rocket, _gfx, 2);

	_crashed = false;

	_GFS = calculateMoonGFS();

	//Flips the rocket so that it can land on the moon
	_rocket->Rotate(180, _gfx->getCentreOfScreen());
	_rocket->getRocketPhysics().velocity.horizontal *= -1;	_rocket->getRocketPhysics().velocity.vertical *= -1;
	
	//Initialises the background music for the level
	_backgroundMusic = new Sound("\\Sound\\MoonBackground.wav", -800, true);
}

void LevelMoon::Unload() {
	_backgroundMusic->Stop();
}

void LevelMoon::procedurallyGenerateGround() {
	for each(CollisionSprite* groundPiece in _ground) {
		groundPiece->setScalePercent(float(_groundScalePercent));
	}

	//Checks if the left-most ground sprite is about to reach a side of the screen and creates a new one next to the left-most sprite
	if (_ground.front()->getSpriteSettings().locationPercent.x > 0 && _rocket->getRocketPhysics().velocity.horizontal < 0) {
		_ground.push_front(new CollisionSprite(*_ground.front()));
		_ground.front()->getSpriteSettings().locationPercent.x -= _groundScalePercent;
		_ground.front()->createRectangularCollisionBox();
	}
	//Checks if the right-most ground sprite is about to reach a side of the screen and creates a new one next to the right-most sprite 
	else if (_ground.back()->getSpriteSettings().locationPercent.x < -100 && _rocket->getRocketPhysics().velocity.horizontal > 0) {
		_ground.push_back(new CollisionSprite(*_ground.back()));
		_ground.back()->getSpriteSettings().locationPercent.x += _groundScalePercent;
		_ground.back()->createRectangularCollisionBox();
	}

	//Checks if the number of ground sprites is above the minimum needed to constantly have a ground sprite on the screen then deletes any that are to far from the rocket
	if (_ground.size() > 3) {
		if (_ground.back()->getSpriteSettings().locationPercent.x < 0 && _rocket->getRocketPhysics().velocity.horizontal > 0) {
			_ground.pop_front();
		}
		else if (_ground.back()->getSpriteSettings().locationPercent.x > -100 && _rocket->getRocketPhysics().velocity.horizontal < 0) {
			_ground.pop_back();
		}
	}
}

void LevelMoon::Update(float totalTime, float deltaTime) {
	UpdateFrame();
	_GFS = calculateMoonGFS();
	_rocket->UpdateFuelMass();
	_spaceInQueue = false;

	//Checks for any key presses
	while (!_keyPressed.empty()) {
		_rocket->CheckRocketInput(_keyPressed, 0.01f, false, _crashed);

		//Returns the user to the main menu when esc pressed
		if (_keyPressed.front() == "Escape") setLevelNo(0);
		//Causes the rocket to detach a stage
		else if (_keyPressed.front() == "Space") {
			if (!_spacePressed) {
				if (_rocket->getRocketEngineOn()) {
					if (_rocket->getCurrentRocketStage().getStageProperties().StageNo > 0) {
						_hud->RemoveCurrentStageHUD();
						//Moves all objects on the screen so that the new centre of the rocket's mass matches the centre of the window
						float centreOfScreenDifference = _rocket->getCurrentRocketStage().getStageProperties().CentreOfRotation.y - _rocket->getRocketStages()[_rocket->getCurrentRocketStage().getStageProperties().StageNo - 1]->getStageProperties().CentreOfRotation.y;

						for each(CollisionSprite* groundPiece in _ground)
							groundPiece->getSpriteSettings().locationPercent.y += centreOfScreenDifference;

						_rocket->DetachStage();
					}
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
		_rocket->getRocketPhysics().force.general = _rocket->getCurrentRocketStage().getStageProperties().MaxPropellingForce;

	if (!_crashed) {
		_rocket->PhysicsCalculations(_rocket->getRocketEngineOn(), _GFS);
		truncateDecimalPlaces(_rocket->getRocketPhysics().velocity.vertical, 4);
		truncateDecimalPlaces(_rocket->getRocketPhysics().velocity.horizontal, 4);

		procedurallyGenerateGround();
		//Updates all sprites so that they remain relative to the position of the rocket
		for (int groundNo = 0; groundNo < _ground.size(); groundNo++) {
			_ground[groundNo]->getSpriteSettings().locationPercent.x -= _rocket->getRocketPhysics().velocity.horizontal / _frHandler->getFrameRate();
			_ground[groundNo]->getSpriteSettings().locationPercent.y += _rocket->getRocketPhysics().velocity.vertical / _frHandler->getFrameRate();
		}
		for (int stageNo = 0; stageNo < _rocket->getRocketStages().size(); stageNo++) {
			_rocket->getRocketStages()[stageNo]->getSpriteSettings().rotatedLocationPercent.x = _rocket->getRocketStages()[stageNo]->getSpriteSettings().locationPercent.x;
			_rocket->getRocketStages()[stageNo]->getSpriteSettings().rotatedLocationPercent.y = _rocket->getRocketStages()[stageNo]->getSpriteSettings().locationPercent.y;

			//Updates the rocket stages' displacement so that it is accurate with the distance the rocket has travelled
			_rocket->getRocketStages()[stageNo]->getSpritePhysics().displacement.vertical += _rocket->getRocketPhysics().velocity.vertical / _frHandler->getFrameRate();
			_rocket->getRocketStages()[stageNo]->getSpritePhysics().displacement.horizontal -= _rocket->getRocketPhysics().velocity.horizontal / _frHandler->getFrameRate();
		}

		//Moves background sprites at a rate which emulates their distance from the rocket
		_moonBackground->getSpriteSettings().locationPercent.x -= _rocket->getRocketPhysics().velocity.horizontal / (_frHandler->getFrameRate() * 500.0f);
		_moonBackground->getSpriteSettings().locationPercent.y += _rocket->getRocketPhysics().velocity.vertical / (_frHandler->getFrameRate() * 210.0f);
		_earth->getSpriteSettings().locationPercent.x -= _rocket->getRocketPhysics().velocity.horizontal / (_frHandler->getFrameRate() * 2000.0f);
		_earth->getSpriteSettings().locationPercent.y += _rocket->getRocketPhysics().velocity.vertical / (_frHandler->getFrameRate() * 5000.0f);
		_stars->getSpriteSettings().locationPercent.x -= _rocket->getRocketPhysics().velocity.horizontal / (_frHandler->getFrameRate() * 2000.0f);
		_stars->getSpriteSettings().locationPercent.y += _rocket->getRocketPhysics().velocity.vertical / (_frHandler->getFrameRate() * 5000.0f);

		//Updates the rocket's displacement so that it is accurate with the distance the rocket has travelled
		_rocket->getRocketPhysics().displacement.vertical += _rocket->getRocketPhysics().velocity.vertical / _frHandler->getFrameRate();
		_rocket->getRocketPhysics().displacement.horizontal -= _rocket->getRocketPhysics().velocity.horizontal / _frHandler->getFrameRate();

		//Slows down the rocket's rotation so that it doesn't remain in constant motion
		_rocket->getRocketPhysics().angularVelocity -= _rocket->getRocketPhysics().angularVelocity / 2000;
		_rocket->Rotate(_rocket->getRocketPhysics().angularVelocity, _gfx->getCentreOfScreen());

		//Checks for collisions between the ground and the rocket stages
		for each(CollisionSprite* rocketStage in _rocket->getRocketStages())
			for each(CollisionSprite* groundPiece in _ground)
				checkCollisions(rocketStage, groundPiece);

		//Checks for the rocket's initial crash
		while (!CollisionSprite::getCollidingCollisionSprites().empty()) {
			CollisionSprite* rocketSprite = CollisionSprite::getCollidingCollisionSprites().front(); CollisionSprite::getCollidingCollisionSprites().pop();
			CollisionSprite* groundSprite = CollisionSprite::getCollidingCollisionSprites().front(); CollisionSprite::getCollidingCollisionSprites().pop();
			//Done if the rocket will explode on impact
			if (abs(_rocket->getRocketPhysics().velocity.vertical) > 7) {
				for each(RocketStage* stage in _rocket->getRocketStages()) {
					stage->getSpritePhysics().velocity.vertical = 0;
					stage->getSpritePhysics().velocity.horizontal = 0;
					stage->ExplodeStage();
				}
				_crashed = true;
				//Resets all values of the rocket due to the rocket now having been destroyed
				_rocket->getRocketEngineOn() = false;
				_rocket->getRocketPhysics().velocity.general = 0;	_rocket->getRocketPhysics().velocity.vertical = 0; _rocket->getRocketPhysics().velocity.horizontal = 0;
				_rocket->getRocketPhysics().force.general = 0;
				_rocket->getRocketPhysics().displacement.vertical = 0;
			}
			//Done if rocket should break part on impact
			else if (abs(_rocket->getRocketPhysics().velocity.vertical) > 4
				|| abs(_rocket->getRocketPhysics().velocity.horizontal) > 0.5f
				|| (_rocket->getRocketStages()[0]->getSpriteSettings().angleFV > 10 && _rocket->getRocketStages()[0]->getSpriteSettings().angleFV < 350)) {
				float angle = _rocket->getRocketSettings().angleFV;
				_rocket->Rotate(-angle, _gfx->getCentreOfScreen());
				PointXY centreOfRotation;
				//Adjusts the centre of rotation for the states so that each sprite can correctly track it's allocated collision box even when each state moves independently 
				//This location will be the starting location of the rocket relative to it's current position
				centreOfRotation.x = ((_rocket->getRocketStartingLocation().x + _rocket->getRocketPhysics().displacement.horizontal) / 100.0f) *  _gfx->getWindowSize().sizeX;
				centreOfRotation.y = ((groundSprite->getSpriteSettings().locationPercent.y) / 100.0f) * _gfx->getWindowSize().sizeY;
				for (int stageNo = 0; stageNo < _rocket->getRocketStages().size(); stageNo++) {
					_rocket->getRocketStages()[stageNo]->Rotate(angle, centreOfRotation);
					_rocket->getRocketStages()[stageNo]->getSpritePhysics().CentreOfRotation = centreOfRotation;
					_rocket->CalculateStageRotatedLocations(_rocket->getRocketStages()[stageNo], centreOfRotation);
					if (_rocket->getRocketStages()[stageNo]->getSpritePhysics().velocity.vertical < 0)
						_rocket->getRocketStages()[stageNo]->getSpritePhysics().velocity.vertical *= -0.25f;
				}
				_crashed = true;
				//Resets all values of the rocket due to the rocket now having been destroyed
				_rocket->getRocketEngineOn() = false;
				_rocket->getRocketPhysics().velocity.general = 0;	_rocket->getRocketPhysics().velocity.vertical = 0; _rocket->getRocketPhysics().velocity.horizontal = 0;
				_rocket->getRocketPhysics().force.general = 0;	_rocket->getRocketPhysics().force.vertical = 0; _rocket->getRocketPhysics().force.horizontal = 0;
				_rocket->getRocketPhysics().displacement.vertical = 0;
			}
			//Done if the rocket is going slow enough to cause a controlled landing
			else if (abs(_rocket->getRocketPhysics().velocity.vertical) <= 4)
				_rocket->getRocketPhysics().velocity.vertical *= -0.25f;
		}
	}
	
	if (_rocket->getExploded()) {
		_respawnTimer++;
		//Blows up any stages that haven't already exploded
		for each(RocketStage* stage in _rocket->getRocketStages()) 
			if (!stage->getExploded()) 
				 stage->ExplodeStage();

		_rocket->getRocketPhysics().velocity.vertical = 0; 	_rocket->getRocketPhysics().velocity.horizontal = 0;
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

	checkRocketGroundCollision(_rocket->getDetachedStages());

	if (_rocket->getRocketEngineOn()) _rocket->UpdateFlames();

	//Checks if the level has been completed and shows the appropriate victory screen
	if (_ground.front()->getSpriteSettings().locationPercent.y < 75 && !_endScreen) {
		_endScreen = true;
		if (abs(_rocket->getRocketPhysics().velocity.vertical) < 20)
			_victoryScreen = createSprite("LevelMoon", "Victory");
		else
			_victoryScreen = createSprite("LevelMoon", "Crashed");
		_victoryScreen->setScalePercent(100);
		_victoryScreen->getSpriteSettings().locationPercent.x = 0;
		_victoryScreen->getSpriteSettings().locationPercent.y = 0;

		_rocket->getRocketPhysics().velocity.vertical = 0; _rocket->getRocketPhysics().velocity.horizontal = 0;
		_backgroundMusic->Stop();
	}

	//Updates the collision boxes
	for each (RocketStage* stage in _rocket->getRocketStages()) stage->Update();
	for each (RocketStage* detachedStage in _rocket->getDetachedStages()) detachedStage->Update();
	for each (CollisionSprite* groundPiece in _ground) groundPiece->Update();
	_hud->Update(_rocket);

	//Respawns the player after 150 frames have occurred after an explosion
	if (_rocket->getExploded() && !_crashed && _respawnTimer > 150) {
		_respawnTimer = 0;
		_rocket = new Rocket(0, float(maxAltitudeFromGround - karmanLine), _gfx, _frHandler);
		_rocketRespawnState.RetrieveSave(*_rocket);
		setLevelNo(5);
	}
}

void LevelMoon::checkRocketGroundCollision(vector<RocketStage*> rocketStages) {
	for each(RocketStage* stage in rocketStages) {
		//Checks for collisions between the ground and stages
		for each(CollisionSprite* groundPiece in _ground)
			checkCollisions(stage, groundPiece);

		while (!CollisionSprite::getCollidingCollisionSprites().empty()) {
			//Removes the two colliding sprites from the vector
			CollisionSprite::getCollidingCollisionSprites().pop();
			CollisionSprite::getCollidingCollisionSprites().pop();
			rocketGroundCollision(stage);
		}
	}
}

void LevelMoon::rocketGroundCollision(RocketStage* stage) {
	//Identifies the point which is the furthest into the other collision sprite
	int mostDeepPoint = 0;
	for (int i = 1; i < 4; i++)
		if (stage->getSpriteCollisionBox()->getBoxCornerLocations()[i].colliding && stage->getSpriteCollisionBox()->getBoxCornerLocations()[i].y > stage->getSpriteCollisionBox()->getBoxCornerLocations()[mostDeepPoint].y)
			mostDeepPoint = i;

	truncateDecimalPlaces(stage->getSpritePhysics().displacement.vertical, 4);

	//Exploded the rocket if the corners 0 or 1 are the ones colliding as that would mean the rocket is upside down or the rocket stage is falling too fast
	if ((mostDeepPoint == 0 || mostDeepPoint == 1 || abs(stage->getSpritePhysics().velocity.vertical) > 10) && !stage->getExploded()) stage->ExplodeStage();

	//Causes the rocket stage to bounce off the ground
	if (stage->getSpritePhysics().velocity.vertical < 0) {
		stage->getSpritePhysics().velocity.vertical *= -0.25f;
		stage->getSpritePhysics().velocity.horizontal *= groundMu;
	}
}


void LevelMoon::Render(float totalTime) {
	//Background Rendering
	D2D1::ColorF* skyColour = new D2D1::ColorF(0, 0, 0, 1);
	_gfx->ClearScreen(*skyColour);
	_stars->Render();
	_earth->Render();
	_moonBackground->Render();

	//Foreground Rendering	
	_rocket->Render(_frame);
	for each (CollisionSprite* groundPiece in _ground) groundPiece->Render();

	//Collision Box Rendering for debug purposes
	if (collisionBoxMode) {
		for each (CollisionSprite* groundPiece in _ground) groundPiece->getSpriteCollisionBox()->renderCollisionBox();
		for each(CollisionSprite* rocketStage in _rocket->getRocketStages()) rocketStage->getSpriteCollisionBox()->renderCollisionBox();
		for each(CollisionSprite* detachedStage in _rocket->getDetachedStages()) detachedStage->getSpriteCollisionBox()->renderCollisionBox();
	}

	//HUD rendering
	_hud->Render(_frame);
	if(_endScreen) _victoryScreen->Render();
}
