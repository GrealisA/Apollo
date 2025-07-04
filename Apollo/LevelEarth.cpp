#include "LevelEarth.h"

void LevelEarth::Load(Graphics* gfx, FrameRateHandler* frHandler) {
	_gfx = gfx;
	_frHandler = frHandler;

	_launchPad[0] = createCollisionSprite("LevelEarth", "RocketSupport", 5.75f, 50000);	
	_launchPad[1] = createCollisionSprite("LevelEarth", "LaunchPad", 12.0f,  50000);

	_rocket = createRocket(0, _launchPad[1]->getSpriteSettings().spriteDimensionsPercent.sizeY);
	//Creates a save point for the rocket at the start of the level
	_rocketRespawnState.Save(_rocket);

	//Initialises the sizes and locations of each sprite
	_launchPad[0]->getSpriteSettings().locationPercent.x = _rocket->getRocketStages()[1]->getSpriteSettings().locationPercent.x + _rocket->getRocketStages()[1]->getSpriteSettings().spriteDimensionsPercent.sizeX + 0.75f;
	_launchPad[0]->getSpriteSettings().locationPercent.y = _rocket->getRocketStartingLocation().y - _launchPad[0]->getSpriteSettings().spriteDimensionsPercent.sizeY;
	_launchPad[0]->createRectangularCollisionBox();

	_launchPad[1]->getSpriteSettings().locationPercent.x = _rocket->getRocketStartingLocation().x + 46.5f;
	_launchPad[1]->getSpriteSettings().locationPercent.y = _rocket->getRocketStartingLocation().y;
	_launchPad[1]->createRectangularCollisionBox();

	_ground.push_front(createCollisionSprite("LevelEarth", "GroundEarth", float(_groundScalePercent), 5.972f * float(pow(10, 24))));
	_ground.front()->getSpriteSettings().locationPercent.x = _rocket->getRocketStartingLocation().x - (_groundScalePercent / 2.0f) + 50;
	_ground.front()->getSpriteSettings().locationPercent.y = _rocket->getRocketStartingLocation().y + _launchPad[1]->getSpriteSettings().spriteDimensionsPercent.sizeY + 0.01f;
	_ground.front()->createRectangularCollisionBox();
	procedurallyGenerateGround();

	//Initialises the clouds and at what altitude they will begin to spawn
	_clouds[0] = new Cloud("Cloud0", 2500, karmanLine, 1, 30, 30);
	_clouds[1] = new Cloud("Cloud1", 2500, karmanLine, 0, 30, 30);

	_groundBackground = createSprite("LevelEarth", "Background Earth");
	_groundBackground->setScalePercent(200);
	_groundBackground->getSpriteSettings().locationPercent.x = 50 - (_groundBackground->getSpriteSettings().spriteDimensionsPercent.sizeX/2.0f);
	_groundBackground->getSpriteSettings().locationPercent.y = 54.5f;

	_groundSecondBackground = new Sprite(*_groundBackground);
	_groundSecondBackground->setScalePercent(300);
	_groundSecondBackground->getSpriteSettings().locationPercent.x = 50 - (_groundSecondBackground->getSpriteSettings().spriteDimensionsPercent.sizeX / 2.0f);
	_groundSecondBackground->getSpriteSettings().locationPercent.y = 63.0f;

	_oceanBackground = createSprite("LevelEarth", "Ocean");
	_oceanBackground->setScalePercent(120);
	_oceanBackground->getSpriteSettings().locationPercent.x = 55 - (_oceanBackground->getSpriteSettings().spriteDimensionsPercent.sizeX / 2.0f);
	_oceanBackground->getSpriteSettings().locationPercent.y = 60;

	_moonBackground = createSprite("LevelEarth", "Moon");
	_moonBackground->setScalePercent(15);
	_moonBackground->getSpriteSettings().locationPercent.x = 70;
	_moonBackground->getSpriteSettings().locationPercent.y = 7;

	_levelTransition = createSprite("LevelEarth", "SpaceTransition");
	_levelTransition->setScalePercent(10000);
	_levelTransition->getSpriteSettings().locationPercent.x = 50 - (_levelTransition->getSpriteSettings().spriteDimensionsPercent.sizeX / 2);
	_levelTransition->getSpriteSettings().locationPercent.y = float(-karmanLine);

	_startGameText = new Text(L"Consolas", 15, _gfx);
	_startGameText->getTSettings().textDimensionsPercent.sizeX = 60;
	_startGameText->getTSettings().textDimensionsPercent.sizeY = 10;
	_startGameText->getTSettings().locationPercent.x = 50 - (_startGameText->getTSettings().textDimensionsPercent.sizeX/2);
	_startGameText->getTSettings().locationPercent.y = 70;
	_startGameText->getTSettings().alignment = "Middle";
	_startGameText->getTSettings().text = "Press Space to ignite rockets";
	_startGameText->getTSettings().colour = D2D1::ColorF::Crimson;

	//Initialisation of the HUD
	_hud = new HUD(_rocket, _gfx, 0);

	_crashed = false;

	_GFS = calculateEarthGFS();
	_rocket->getRocketPhysics().displacement.vertical = 0;

	//Initialises the background music for the level
	_backgroundMusic = new Sound("\\Sound\\EarthBackground.wav", -800, true);
}

void LevelEarth::Unload() {
	_backgroundMusic->Stop();
	_hud->~HUD();
}

void LevelEarth::procedurallyGenerateGround() {
	for each(CollisionSprite* groundPiece in _ground) {
		groundPiece->setScalePercent(float(_groundScalePercent));
	}

	//Checks if the furthest ground sprite is about to reach a side of the screen and creates a new one next to the furthest sprite
	if (_ground.front()->getSpriteSettings().locationPercent.x > 0 && _rocket->getRocketPhysics().velocity.horizontal < 0) {
		_ground.push_front(new CollisionSprite(*_ground.front()));
		_ground.front()->getSpriteSettings().locationPercent.x -= _groundScalePercent;
		_ground.front()->createRectangularCollisionBox();
	}
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

void LevelEarth::Update(float totalTime, float deltaTime) {
	UpdateFrame();
	_GFS = calculateEarthGFS();
	_rocket->UpdateFuelMass();
	_spaceInQueue = false;

	//Checks for any key presses
	while (!_keyPressed.empty()) {
		_rocket->CheckRocketInput(_keyPressed, 0.005f, false, _crashed);
		
		//Returns the user to the main menu when esc pressed
		if (_keyPressed.front() == "Escape") setLevelNo(0);
		//Causes the rocket to take off or detaches a stage if it has already launched
		else if (_keyPressed.front() == "Space") {
			if (!_spacePressed) {
				if (!_rocket->getRocketEngineOn()) {
					_gameStarted = true;
					_rocket->getRocketEngineOn() = true;
					_rocket->getRocketPhysics().force.general = _rocket->getCurrentRocketStage().getStageProperties().MaxPropellingForce;
				}
				else {
					if (_rocket->getCurrentRocketStage().getStageProperties().StageNo != 0) {
						_hud->RemoveCurrentStageHUD();

						//Moves all objects on the screen so that the new centre of the rocket's mass matches the centre of the window
						float centreOfScreenDifference = _rocket->getCurrentRocketStage().getStageProperties().CentreOfRotation.y - _rocket->getRocketStages()[_rocket->getCurrentRocketStage().getStageProperties().StageNo - 1]->getStageProperties().CentreOfRotation.y;

						for each(CollisionSprite* groundPiece in _ground)
							groundPiece->getSpriteSettings().locationPercent.y += centreOfScreenDifference;
						for each(CollisionSprite* launchPadPiece in _launchPad)
							launchPadPiece->getSpriteSettings().locationPercent.y += centreOfScreenDifference;

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

	if (!_crashed && !_rocket->getExploded()) {	
		_rocket->PhysicsCalculations(_rocket->getRocketEngineOn(), _GFS);
		truncateDecimalPlaces(_rocket->getRocketPhysics().velocity.vertical, 4);
		truncateDecimalPlaces(_rocket->getRocketPhysics().velocity.horizontal, 4);
		
		procedurallyGenerateGround();
		//Updates all sprites so that they remain relative to the position of the rocket
		for (int groundNo = 0; groundNo < _ground.size(); groundNo++) {
			_ground[groundNo]->getSpriteSettings().locationPercent.x -= _rocket->getRocketPhysics().velocity.horizontal/ _frHandler->getFrameRate();
			_ground[groundNo]->getSpriteSettings().locationPercent.y += _rocket->getRocketPhysics().velocity.vertical / _frHandler->getFrameRate();
		}
 		for (int stageNo = 0; stageNo < _rocket->getRocketStages().size(); stageNo++) {
			_rocket->getRocketStages()[stageNo]->getSpriteSettings().rotatedLocationPercent.x = _rocket->getRocketStages()[stageNo]->getSpriteSettings().locationPercent.x;
			_rocket->getRocketStages()[stageNo]->getSpriteSettings().rotatedLocationPercent.y = _rocket->getRocketStages()[stageNo]->getSpriteSettings().locationPercent.y;
			//Updates the rocket stages' displacement so that it is accurate with the distance the rocket has travelled
			_rocket->getRocketStages()[stageNo]->getSpritePhysics().displacement.vertical += _rocket->getRocketPhysics().velocity.vertical / _frHandler->getFrameRate();
			_rocket->getRocketStages()[stageNo]->getSpritePhysics().displacement.horizontal -= _rocket->getRocketPhysics().velocity.horizontal / _frHandler->getFrameRate();
		}
		for (int launchPadNo = 0; launchPadNo < size(_launchPad); launchPadNo++) {
			_launchPad[launchPadNo]->getSpriteSettings().locationPercent.x -= _rocket->getRocketPhysics().velocity.horizontal/ _frHandler->getFrameRate();
			_launchPad[launchPadNo]->getSpriteSettings().locationPercent.y += _rocket->getRocketPhysics().velocity.vertical / _frHandler->getFrameRate();
		}
		for (int cloudNo = 0; cloudNo < size(_clouds); cloudNo++) {
			_clouds[cloudNo]->GenerateClouds(_rocket->getRocketPhysics(), _rocket->getRocketSettings());
			_clouds[cloudNo]->UpdateClouds(_rocket->getRocketPhysics().velocity, _rocket->getRocketSettings(), _frHandler->getFrameRate());
		}

		//Moves background sprites at a rate which emulates their distance from the rocket
		_groundBackground->getSpriteSettings().locationPercent.x -= _rocket->getRocketPhysics().velocity.horizontal / (_frHandler->getFrameRate() * 120.0f);
		_groundBackground->getSpriteSettings().locationPercent.y += _rocket->getRocketPhysics().velocity.vertical / (_frHandler->getFrameRate() * 50.0f);
		_groundSecondBackground->getSpriteSettings().locationPercent.x -= _rocket->getRocketPhysics().velocity.horizontal / (_frHandler->getFrameRate() * 100.0f);
		_groundSecondBackground->getSpriteSettings().locationPercent.y += _rocket->getRocketPhysics().velocity.vertical / (_frHandler->getFrameRate() * 20.0f);
		_oceanBackground->getSpriteSettings().locationPercent.x -= _rocket->getRocketPhysics().velocity.horizontal / (_frHandler->getFrameRate() * 500.0f);
		_oceanBackground->getSpriteSettings().locationPercent.y += _rocket->getRocketPhysics().velocity.vertical / (_frHandler->getFrameRate() * 5000.0f);
		_moonBackground->getSpriteSettings().locationPercent.x -= _rocket->getRocketPhysics().velocity.horizontal / (_frHandler->getFrameRate() * 5000.0f);
		_moonBackground->getSpriteSettings().locationPercent.y += _rocket->getRocketPhysics().velocity.vertical / (_frHandler->getFrameRate() * 4000.0f);
		_levelTransition->getSpriteSettings().locationPercent.x -= _rocket->getRocketPhysics().velocity.horizontal / _frHandler->getFrameRate();
		_levelTransition->getSpriteSettings().locationPercent.y += _rocket->getRocketPhysics().velocity.vertical / _frHandler->getFrameRate();

		//Updates the rocket's displacement so that it is accurate with the distance the rocket has travelled
		_rocket->getRocketPhysics().displacement.vertical += _rocket->getRocketPhysics().velocity.vertical / _frHandler->getFrameRate();
		_rocket->getRocketPhysics().displacement.horizontal -= _rocket->getRocketPhysics().velocity.horizontal / _frHandler->getFrameRate();

		//Slows down the rocket's rotation so that it doesn't remain in constant motion
		_rocket->getRocketPhysics().angularVelocity -= _rocket->getRocketPhysics().angularVelocity / 100;
		_rocket->Rotate(_rocket->getRocketPhysics().angularVelocity, _gfx->getCentreOfScreen());

		//Checks for collisions between the ground and the rocket stages
		for each(CollisionSprite* rocketStage in _rocket->getRocketStages())
			for each(CollisionSprite* groundPiece in _ground)
				checkCollisions(rocketStage, groundPiece);

		//Checks for collisions between the launchpad and the rocket stages
		for each(CollisionSprite* rocketStage in _rocket->getRocketStages())
			checkCollisions(rocketStage, _launchPad[1]);

		//Checks for the rocket's initial crash
		while (!CollisionSprite::getCollidingCollisionSprites().empty()) {
			CollisionSprite* rocketSprite = CollisionSprite::getCollidingCollisionSprites().front(); CollisionSprite::getCollidingCollisionSprites().pop();
			CollisionSprite* groundSprite = CollisionSprite::getCollidingCollisionSprites().front(); CollisionSprite::getCollidingCollisionSprites().pop();
			//Done if the rocket will explode on impact
			if (abs(_rocket->getRocketPhysics().velocity.vertical) > 7) {
				for each(RocketStage* stage in _rocket->getRocketStages()){
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

				//Adjusts the centre of rotation for the states so that each sprite can correctly track it's allocated collision box even when each state moves independently 
				//This location will be the starting location of the rocket relative to it's current position
				PointXY centreOfRotation;
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

		//Checks for collisions between the rocket stages and the clouds
		for each(CollisionSprite* rocketStage in _rocket->getRocketStages())
			for(int cloudID = 0; cloudID < size(_clouds); cloudID++)
				for each(CollisionSprite* cloud in _clouds[cloudID]->getClouds())
					checkCollisions(rocketStage, cloud);
		//Decreases the speed of the rocket for each frame that the rocket is in the cloud
		if (!CollisionSprite::getCollidingCollisionSprites().empty()) {
			_rocket->getRocketPhysics().velocity.vertical -= 0.002f * (_rocket->getRocketPhysics().velocity.vertical + _GFS);
			//Ensures there is only one collision at a time between the rocket and the clouds
			while (!CollisionSprite::getCollidingCollisionSprites().empty()) CollisionSprite::getCollidingCollisionSprites().pop();
		}
	}

	for (int detachedStageNo = 0; detachedStageNo < _rocket->getDetachedStages().size(); detachedStageNo++){
		//Allows gravity to affect the detached stage until it explodes
		if (!_rocket->getDetachedStages()[detachedStageNo]->getExploded())
			_rocket->getDetachedStages()[detachedStageNo]->getSpritePhysics().velocity.vertical -= _GFS / _frHandler->getFrameRate();
		else
			_rocket->getDetachedStages()[detachedStageNo]->getSpritePhysics().velocity.vertical = 0;
		
		//Updates the location of the selected detached stage
		_rocket->getDetachedStages()[detachedStageNo]->getSpritePhysics().displacement.horizontal += (_rocket->getDetachedStages()[detachedStageNo]->getSpritePhysics().velocity.horizontal - _rocket->getRocketPhysics().velocity.horizontal) / _frHandler->getFrameRate();
		_rocket->getDetachedStages()[detachedStageNo]->getSpritePhysics().displacement.vertical -= (_rocket->getDetachedStages()[detachedStageNo]->getSpritePhysics().velocity.vertical - _rocket->getRocketPhysics().velocity.vertical) / _frHandler->getFrameRate();
		_rocket->getDetachedStages()[detachedStageNo]->getSpriteSettings().locationPercent.x += (_rocket->getDetachedStages()[detachedStageNo]->getSpritePhysics().velocity.horizontal - _rocket->getRocketPhysics().velocity.horizontal) / _frHandler->getFrameRate();
		_rocket->getDetachedStages()[detachedStageNo]->getSpriteSettings().locationPercent.y -= (_rocket->getDetachedStages()[detachedStageNo]->getSpritePhysics().velocity.vertical - _rocket->getRocketPhysics().velocity.vertical) / _frHandler->getFrameRate();
		_rocket->CalculateStageRotatedLocations(_rocket->getDetachedStages()[detachedStageNo], _gfx->getCentreOfScreen());
	}
	
	checkRocketGroundCollision(_rocket->getDetachedStages());

	if(_crashed){
		_respawnTimer++;

		//Adjusts the locations each stage under the effects of gravity
		for each(RocketStage* stage in _rocket->getRocketStages()){
			stage->getSpritePhysics().acceleration.vertical = -_GFS;
			stage->getSpritePhysics().velocity.vertical += stage->getSpritePhysics().acceleration.vertical / _frHandler->getFrameRate();
		}

		checkRocketGroundCollision(_rocket->getRocketStages());

		rocketCrash();
		_rocket->getRocketEngineOn() = false;
	}

	if (_rocket->getExploded()) {
		//Blows up any stages that haven't already exploded
		for each(RocketStage* stage in _rocket->getRocketStages())
			if (!stage->getExploded()) stage->ExplodeStage();
		_respawnTimer++;
	}

	if(_rocket->getRocketEngineOn()) _rocket->UpdateFlames();

	//Updates the collision boxes
	for each (RocketStage* stage in _rocket->getRocketStages()) stage->Update();
	for each (RocketStage* detachedStage in _rocket->getDetachedStages()) detachedStage->Update();
	for each (CollisionSprite* groundPiece in _ground) groundPiece->Update();
	for (int launchPadNo = 0; launchPadNo < 2; launchPadNo++) _launchPad[launchPadNo]->Update();

	_hud->Update(_rocket);

	//Switches to the space level when the player reaches the karman line
	if (_rocket->getRocketPhysics().displacement.vertical > karmanLine)
		setLevelNo(4);
	//Respawns the player after 150 frames have occurred after a crash
	else if ((_crashed || _rocket->getExploded()) && _respawnTimer > 150) {
		_respawnTimer = 0;
		_rocket = new Rocket(0, 0, _gfx, _frHandler);
		_rocketRespawnState.RetrieveSave(*_rocket);
		setLevelNo(1);
	}
}

void LevelEarth::checkRocketGroundCollision(vector<RocketStage*> rocketStages) {
	for each(RocketStage* stage in rocketStages) {
		//Checks for collisions between the launchpad and rocket stages
		checkCollisions(stage, _launchPad[1]);

		while (!CollisionSprite::getCollidingCollisionSprites().empty()) {
			//Removes the two colliding sprites from the vector
			CollisionSprite::getCollidingCollisionSprites().pop();
			CollisionSprite::getCollidingCollisionSprites().pop();
			rocketGroundCollision(stage);
		}

		//Checks for collisions between the ground and rocket stages
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

void LevelEarth::rocketGroundCollision(RocketStage* stage) {
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


void LevelEarth::Render(float totalTime) {
	//Background Rendering
	float colour[4];
	colour[0] = 4 * (1 -(_rocket->getRocketPhysics().displacement.vertical/ (karmanLine))) + 10;
	colour[1] = 7 * (1 - (_rocket->getRocketPhysics().displacement.vertical / (karmanLine))) + 12;
	colour[2] = 28 * (1 - (_rocket->getRocketPhysics().displacement.vertical / (karmanLine))) + 16;
	colour[3] = 1;
	for (int count = 0; count < 3; count++) if (colour[count] < 0) colour[count] = 0;
	D2D1::ColorF* skyColour = new D2D1::ColorF(colour[0], colour[1], colour[2], colour[3]);
	_gfx->ClearScreen(*skyColour);
	_moonBackground->Render();
	_oceanBackground->Render();
	_groundBackground->Render();
	_groundSecondBackground->Render();

	//Foreground Rendering	
	_rocket->Render(_frame);
	for each (CollisionSprite* launchPadPiece in _launchPad) launchPadPiece->Render();
	for each (CollisionSprite* groundPiece in _ground) groundPiece->Render();
	for each(Cloud* cloud in _clouds) cloud->RenderClouds();

	//Collision Box Rendering for debug purposes
	if (collisionBoxMode) {
		for each (CollisionSprite* groundPiece in _ground) groundPiece->getSpriteCollisionBox()->renderCollisionBox();
		for each(CollisionSprite* rocketStage in _rocket->getRocketStages()) rocketStage->getSpriteCollisionBox()->renderCollisionBox();
		for each(CollisionSprite* detachedStage in _rocket->getDetachedStages()) detachedStage->getSpriteCollisionBox()->renderCollisionBox();
		for (int launchPadNo = 0; launchPadNo < 2; launchPadNo++) _launchPad[launchPadNo]->getSpriteCollisionBox()->renderCollisionBox();
	}

	//HUD rendering
	if(!_gameStarted) _startGameText->Render();
	_hud->Render(_frame);
	_levelTransition->Render();
}

void LevelEarth::rocketCrash() {
	for (int stageNo = 0; stageNo < _rocket->getRocketStages().size(); stageNo++) {
		if (!_rocket->getRocketStages()[stageNo]->getExploded()) {
			//Updates the displacement of each of the stages after impact
			_rocket->getRocketStages()[stageNo]->getSpritePhysics().displacement.vertical += (_rocket->getRocketStages()[stageNo]->getSpritePhysics().velocity.vertical/ _frHandler->getFrameRate());
			_rocket->getRocketStages()[stageNo]->getSpritePhysics().displacement.horizontal += (_rocket->getRocketStages()[stageNo]->getSpritePhysics().velocity.horizontal / _frHandler->getFrameRate());
			
			truncateDecimalPlaces(_rocket->getRocketStages()[stageNo]->getSpritePhysics().displacement.vertical, 4);
			truncateDecimalPlaces(_rocket->getRocketStages()[stageNo]->getSpritePhysics().displacement.horizontal, 4);

			//Moves the stages' sprite to the necessary position on the screen
			_rocket->getRocketStages()[stageNo]->getSpriteSettings().locationPercent.x += (_rocket->getRocketStages()[stageNo]->getSpritePhysics().velocity.horizontal / _frHandler->getFrameRate());
			_rocket->getRocketStages()[stageNo]->getSpriteSettings().locationPercent.y -= (_rocket->getRocketStages()[stageNo]->getSpritePhysics().velocity.vertical / _frHandler->getFrameRate());

			//In the event of the rocket breaking apart, the stage sprites are placed in positions which allows them to track their allocated collision box when rotated
			PointXY centreOfRotation;
			centreOfRotation.x = ((_rocket->getRocketStartingLocation().x + _rocket->getRocketPhysics().displacement.horizontal) / 100) *  _gfx->getWindowSize().sizeX;
			centreOfRotation.y = ((_ground.front()->getSpriteSettings().locationPercent.y) / 100) * _gfx->getWindowSize().sizeY;

			_rocket->CalculateStageRotatedLocations(_rocket->getRocketStages()[stageNo], centreOfRotation);

			truncateDecimalPlaces(_rocket->getRocketStages()[stageNo]->getSpriteSettings().rotatedLocationPercent.x, 4);
			truncateDecimalPlaces(_rocket->getRocketStages()[stageNo]->getSpriteSettings().rotatedLocationPercent.y, 4);
		}
	}
}
