#include "HUD.h"

Sound* HUD::_sfx;

HUD::HUD(Rocket* rocket, Graphics* gfx, int levelNo){
	_noStages = int(rocket->getRocketStages().size());

	_gfx = gfx;
	_opacityAlternater = 1;
	_levelNo = levelNo;

	_physicsOutput = new Sprite("Sprites\\HUD\\Instruments.png", "Instruments");
	_physicsOutput->setScalePercent(15);
	_physicsOutput->getSpriteSettings().locationPercent.x = 84;
	_physicsOutput->getSpriteSettings().locationPercent.y = 70;

	//Allocating text in the HUD to their appropriate locations
	PointXY tempTextLocation; 
	SizeXY tempTextProportions;
	tempTextLocation.x = _physicsOutput->getSpriteSettings().locationPercent.x + (_physicsOutput->getSpriteSettings().spriteDimensionsPercent.sizeX / 2.35f); 
	tempTextLocation.y = _physicsOutput->getSpriteSettings().locationPercent.y + (_physicsOutput->getSpriteSettings().spriteDimensionsPercent.sizeX / 9);
	tempTextProportions.sizeX = 10;
	tempTextProportions.sizeY = 5;
	_altitudeLabelText = setupText("Altitude:", 3.5f, tempTextLocation, tempTextProportions);

	tempTextLocation.y += _physicsOutput->getSpriteSettings().spriteDimensionsPercent.sizeX / 30;

	_altitudeText = setupText(to_string(rocket->getRocketPhysics().displacement.vertical) + "m", 10, tempTextLocation, tempTextProportions);
	
	tempTextLocation.y += _physicsOutput->getSpriteSettings().spriteDimensionsPercent.sizeX / 1.5f;

	_vVelocityLabelText = setupText("Vertical Velocity:", 3.5f , tempTextLocation, tempTextProportions);

	tempTextLocation.y += _physicsOutput->getSpriteSettings().spriteDimensionsPercent.sizeX / 30;

	_vVelocityText = setupText(to_string(rocket->getRocketPhysics().velocity.vertical) + "m/s", 10, tempTextLocation, tempTextProportions);

	tempTextLocation.y += _physicsOutput->getSpriteSettings().spriteDimensionsPercent.sizeX / 1.5f;

	_hVelocityLabelText = setupText("Horizontal Velocity:", 3.5f, tempTextLocation, tempTextProportions);
	tempTextLocation.y += _physicsOutput->getSpriteSettings().spriteDimensionsPercent.sizeX / 30;

	_hVelocityText = setupText(to_string(rocket->getRocketPhysics().velocity.horizontal) + "m/s", 10, tempTextLocation, tempTextProportions);

	//Setting up of HUD sprites with allocation of sizes and locations
	_throttleKnob = new Sprite("Sprites\\HUD\\Throttle knob.png", "Throttle knob");
	_throttleKnob->setScalePercent(3);
	_throttleKnob->getSpriteSettings().locationPercent.x = _physicsOutput->getSpriteSettings().locationPercent.x + 0.75f;
	_throttleKnob->getSpriteSettings().locationPercent.y = _physicsOutput->getSpriteSettings().locationPercent.y + _physicsOutput->getSpriteSettings().spriteDimensionsPercent.sizeY - _throttleKnob->getSpriteSettings().spriteDimensionsPercent.sizeY + 0.5f;

	_map = new SpriteSheet("SpriteSheet\\HUD\\Map.png", "Map", 90, 720);
	_map->setScalePercent(5);
	_map->getSpriteSettings().locationPercent.x = 1;
	_map->getSpriteSettings().locationPercent.y = (100 - _map->getSpriteSettings().spriteDimensionsPercent.sizeY) / 2;
	
	_satelliteIndicator = new Sprite("Sprites\\HUD\\SatelliteMarker.png", "SatelliteMarker");
	_satelliteIndicator->setScalePercent(0.25f);
	_satelliteIndicator->getSpriteSettings().opacity = 0;
	
	_mapIndicator = new Sprite("Sprites\\HUD\\LocationCross.png", "Indicator");
	_mapIndicator->setScalePercent(0.5f);
	_mapIndicator->getSpriteSettings().locationPercent.x = _map->getSpriteSettings().locationPercent.x + (_map->getSpriteSettings().spriteDimensionsPercent.sizeX/2) - (_mapIndicator->getSpriteSettings().spriteDimensionsPercent.sizeX/2);
	_mapIndicator->getSpriteSettings().locationPercent.y = _map->getSpriteSettings().locationPercent.y + _map->getSpriteSettings().spriteDimensionsPercent.sizeY - (_mapIndicator->getSpriteSettings().spriteDimensionsPercent.sizeY / 2);

	//Initialises the HUD warnings 
	tempTextProportions.sizeX = 30;
	_trajectoryWarning = setupText("LEAVING LUNAR\nTRAJECTORY", 15.0f, tempTextLocation, tempTextProportions);
	_trajectoryWarning->getTSettings().font = L"Consolas";
	_trajectoryWarning->getTSettings().alignment = "Middle";
	_trajectoryWarning->getTSettings().colour = D2D1::ColorF::Crimson;
	_trajectoryWarning->getTSettings().locationPercent.x = 50 - (_trajectoryWarning->getTSettings().textDimensionsPercent.sizeX / 2);
	_trajectoryWarning->getTSettings().locationPercent.y = 10;
	
	tempTextProportions.sizeX = 17.7f;
	_stallWarning = setupText("STALLING", 20.0f, tempTextLocation, tempTextProportions);
	_stallWarning->getTSettings().font = L"Consolas";
	_stallWarning->getTSettings().colour = D2D1::ColorF::Crimson;
	_stallWarning->getTSettings().locationPercent.x = 50 - (_stallWarning->getTSettings().textDimensionsPercent.sizeX / 2);
	_stallWarning->getTSettings().locationPercent.y = 10;

	//Initialises the mission log
	tempTextLocation.x = 1;
	tempTextLocation.y = 1;
	tempTextProportions.sizeX = 15;
	tempTextProportions.sizeY = 6;
	_missionText = setupText("", 5.5f, tempTextLocation, tempTextProportions);
	_missionText->getTSettings().font = L"Consolas";
	_missionText->getTSettings().colour = D2D1::ColorF::WhiteSmoke;


	//Initialises the rocket fuel gauges 
	_stagesFuel.resize(_noStages);
	_stagesFuelCurrentFrame.resize(_noStages);
	for (int stageNo = 0; stageNo < _stagesFuel.size(); stageNo++) {
		int frameHeight, frameWidth;
		switch (stageNo) {
		case 0:
			frameWidth = 66; frameHeight = 470;
			break;
		case 1:
			frameWidth = 130; frameHeight = 345;
			break;
		case 2:
			frameWidth = 152; frameHeight = 450;
			break;
		}
		_stagesFuel[stageNo] = new SpriteSheet("SpriteSheet\\HUD\\Stage " + to_string(stageNo) + " Fuel Indicator.png", "Stage " + to_string(stageNo) + "Fuel Indicator", frameWidth, frameHeight);
		_stagesFuel[stageNo]->getSpriteSettings() = rocket->getRocketStages()[stageNo]->getSpriteSettings();
		_stagesFuel[stageNo]->getSpriteSettings().angleFV = 0;
		_stagesFuel[stageNo]->getSpriteSettings().rotationMatrix = D2D1::Matrix3x2F::Identity();
		_stagesFuel[stageNo]->getSpriteSettings().locationPercent.x -= 41;
		_stagesFuel[stageNo]->getSpriteSettings().opacity = 0.8f;
		_stagesFuelCurrentFrame[stageNo] = int((1 - (rocket->getRocketStages()[stageNo]->getStageProperties().FuelMass/rocket->getRocketStages()[stageNo]->getStageProperties().FuelMaxCapacity)) * _stagesFuel[stageNo]->getNoOfFrames());
	}
}

HUD::~HUD(){
	if(_sfx) _sfx->Stop();
}

void HUD::RemoveCurrentStageHUD() {
	//Removes the bottom most fuel gauges from the HUD
	_stagesFuel.pop_back();
	_stagesFuelCurrentFrame.pop_back();
}

//Calculates the location of the sprite on the map depending on its actual displacement
PointXY HUD::CalculateLocationOnMap(VectorQuantity spriteDisplacement) {
	PointXY location;
	//Calculates the location of the sprite on the map in the x axis
	location.x = _map->getSpriteSettings().locationPercent.x
		+ (_map->getSpriteSettings().spriteDimensionsPercent.sizeX / 2) - (_mapIndicator->getSpriteSettings().spriteDimensionsPercent.sizeX / 2)
		- ((spriteDisplacement.horizontal / maxHorizontalDisplacement) * (_map->getSpriteSettings().spriteDimensionsPercent.sizeX / 2));
	
	//Calculates the location of the sprite on the map in the y axis
	if (_levelNo != 2) {
		location.y = _map->getSpriteSettings().locationPercent.y
			+ _map->getSpriteSettings().spriteDimensionsPercent.sizeY - (_mapIndicator->getSpriteSettings().spriteDimensionsPercent.sizeY / 2)
			- ((spriteDisplacement.vertical / maxAltitudeFromGround) * _map->getSpriteSettings().spriteDimensionsPercent.sizeY);
	}	
	//Due to the moon level taking place upside down, the calculations are changed to match this
	else {
		location.y = _map->getSpriteSettings().locationPercent.y
			+ _map->getSpriteSettings().spriteDimensionsPercent.sizeY - (_mapIndicator->getSpriteSettings().spriteDimensionsPercent.sizeY / 2)
			- (((maxAltitudeFromGround - spriteDisplacement.vertical) / maxAltitudeFromGround) * _map->getSpriteSettings().spriteDimensionsPercent.sizeY);
	}
	return location;
}

void HUD::Update(Rocket* rocket) {

	//Checks if the stall warning needs to be shown
	if ((rocket->getRocketSettings().angleFV > 300 || rocket->getRocketSettings().angleFV < 60)
		&& rocket->getRocketPhysics().velocity.vertical < 0 && !_showStallWarning && !rocket->getExploded() && _levelNo == 0) {
		_showStallWarning = true;
		//An alarm is played to inform the user of the stall warning
		_sfx = new Sound("\\Sound\\Alarm.wav", -500, true);
	}
	else if (_showStallWarning && rocket->getRocketPhysics().velocity.vertical >= 0) {
		if (_sfx) _sfx->Stop();
		_showStallWarning = false;
	}

	//Checks if the flight path warning needs to be shown
	if (abs(rocket->getRocketPhysics().displacement.horizontal) > maxHorizontalDisplacement || rocket->getExploded()) {
		rocket->getExploded() = true;
		if (_sfx) _sfx->Stop();
		_showTrajectoryWarning = false;
		_showStallWarning = false;
	}
	else if (abs(rocket->getRocketPhysics().displacement.horizontal) > 4 * maxHorizontalDisplacement / 6 && !_showTrajectoryWarning) {
		_showTrajectoryWarning = true;
		//An alarm is played to inform the user of the flight path warning
		_sfx = new Sound("\\Sound\\Alarm.wav", -500, true);
	}
	else if (_showTrajectoryWarning && abs(rocket->getRocketPhysics().displacement.horizontal) <= 4 * maxHorizontalDisplacement / 6){
		if (_sfx)_sfx->Stop();
		_showTrajectoryWarning = false;
	}

	//Sets the mission log depending on which level is currently being played
	if (_levelNo == 0 && rocket->getRocketPhysics().displacement.vertical > 200)
		_missionText->getTSettings().text = "Current Mission:\nReach Space at 100km altitude\nDodge clouds to avoid stalling";
	else if (_levelNo == 1)
		_missionText->getTSettings().text = "Current Mission:\nReach the moon to start descent\nCollect satellites to regain fuel\nHold Control to active speed brakes";
	else if (_levelNo == 2)
		_missionText->getTSettings().text = "Current Mission:\nLand on the moon";

	//Calculates the location of the throttle knob relative to the throttle backdrop to show the amount of throttle being used
	_throttleKnob->getSpriteSettings().locationPercent.y = 
		_physicsOutput->getSpriteSettings().locationPercent.y + _physicsOutput->getSpriteSettings().spriteDimensionsPercent.sizeY - _throttleKnob->getSpriteSettings().spriteDimensionsPercent.sizeY + 0.5f 
		- ((_physicsOutput->getSpriteSettings().spriteDimensionsPercent.sizeY - _throttleKnob->getSpriteSettings().spriteDimensionsPercent.sizeY + 1) 
		* (rocket->getRocketPhysics().force.general / rocket->getCurrentRocketStage().getStageProperties().MaxPropellingForce));
	
	//Calculates the frame of the spritesheet needed to correctly convey the amount of fuel left in the rocket to the user
	for (int stageNo = 0; stageNo < rocket->getRocketStages().size(); stageNo++) {
		_stagesFuelCurrentFrame[stageNo] =
			int((1 - (rocket->getRocketStages()[stageNo]->getStageProperties().FuelMass / rocket->getRocketStages()[stageNo]->getStageProperties().FuelMaxCapacity)) * (_stagesFuel[stageNo]->getNoOfFrames() - 1));
	}

	_mapIndicator->getSpriteSettings().locationPercent = CalculateLocationOnMap(rocket->getRocketPhysics().displacement);

	//Alternates the opacity of the warnings
	if (_showTrajectoryWarning || _showStallWarning) {
		_trajectoryWarning->getTSettings().opacity = (sin(_opacityAlternater) + 1) / 2;
		_stallWarning->getTSettings().opacity = (sin(_opacityAlternater) + 1) / 2;
		_opacityAlternater += 0.25f;
	}

	//Changes the altitude units from m to km when above 1000m
	if (rocket->getRocketPhysics().displacement.vertical < 1000)
		_altitudeText->getTSettings().text = to_string(int(rocket->getRocketPhysics().displacement.vertical)) + "m";
	else 		
		_altitudeText->getTSettings().text = to_string(int(rocket->getRocketPhysics().displacement.vertical/1000)) + "km";

	//Changes the velocity units from m/s to km/s when above 1000m/s
	if (rocket->getRocketPhysics().velocity.vertical < 1000)
		_vVelocityText->getTSettings().text = to_string(int(rocket->getRocketPhysics().velocity.vertical)) + "m/s";
	else
		_vVelocityText->getTSettings().text = to_string(int(rocket->getRocketPhysics().velocity.vertical/ 1000)) + "km/s";
	if (rocket->getRocketPhysics().velocity.horizontal < 1000)
		_hVelocityText->getTSettings().text = to_string(int(rocket->getRocketPhysics().velocity.horizontal)) + "m/s";
	else
		_hVelocityText->getTSettings().text = to_string(int(rocket->getRocketPhysics().velocity.horizontal/ 1000)) + "km/s";
}

void HUD::Update(Rocket* rocket, Satellite* satellite) {
	Update(rocket);

	_satelliteIndicator->getSpriteSettings().opacity = 1;
	_satelliteIndicator->getSpriteSettings().locationPercent = CalculateLocationOnMap(satellite->getSpritePhysics().displacement);
}

void HUD::Render(int frame) {
	//Renders all sprites in the HUD
	_physicsOutput->Render();
	_throttleKnob->Render();
	_map->Render(_levelNo);
	if (_satelliteIndicator) _satelliteIndicator->Render();
	_mapIndicator->Render();
	for (int stageNo = 0; stageNo < _stagesFuel.size(); stageNo++)
		_stagesFuel[stageNo]->Render(_stagesFuelCurrentFrame[stageNo]);
	_altitudeLabelText->Render();
	_altitudeText->Render();
	_vVelocityLabelText->Render();
	_vVelocityText->Render();
	_hVelocityLabelText->Render();
	_hVelocityText->Render();
	_missionText->Render();
	if(_showTrajectoryWarning)
		_trajectoryWarning->Render();
	if (_showStallWarning)
		_stallWarning->Render();
}

//Initialises and returns text specified by the parameters
Text* HUD::setupText(string text, float fontSize, PointXY location, SizeXY Proportions) {
	Text* newText = new Text(L"Consolas", fontSize, _gfx);
	newText->getTSettings().locationPercent = location;
	newText->getTSettings().textDimensionsPercent = Proportions;
	newText->getTSettings().text = text;
	newText->getTSettings().colour = D2D1::ColorF::AliceBlue;
	return newText;
}
