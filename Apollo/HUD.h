#pragma once

#include "Text.h"
#include "Rocket.h"
#include "Satellite.h"
#include <thread>
#include <vector>

using namespace std;

class HUD
{
public:
	HUD(Rocket* rocket, Graphics* gfx, int levelNo);
	~HUD();
	
	void Update(Rocket* rocket);
	void Update(Rocket* rocket, Satellite* satellite);
	void Render(int frame);	
	void RemoveCurrentStageHUD();
	PointXY CalculateLocationOnMap(VectorQuantity spriteDisplacement);

private:
	Text* setupText(string text, float fontSize, PointXY location, SizeXY Proportions);

	Graphics* _gfx;
	int _noStages;
	float _opacityAlternater;
	bool _showTrajectoryWarning = false;
	bool _showStallWarning = false;
	SpriteSheet* _map;	
	vector<SpriteSheet*> _stagesFuel;
	Sprite* _physicsOutput;
	Sprite* _throttleKnob;
	Sprite* _mapIndicator;	
	Sprite* _satelliteIndicator;

	vector<int> _stagesFuelCurrentFrame;
	Text* _missionText;
	Text* _altitudeText;
	Text* _vVelocityText;
	Text* _hVelocityText;
	Text* _altitudeLabelText;
	Text* _vVelocityLabelText;
	Text* _hVelocityLabelText;	
	Text* _trajectoryWarning;
	Text* _stallWarning;
	int _levelNo;
	static Sound* _sfx;
};

