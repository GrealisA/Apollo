#pragma once
#include "Level.h"
#include "HUD.h"
#include "Cloud.h"

class LevelEarth: public Level {
	using Level::Level;
public:
	void Load(Graphics* gfx, FrameRateHandler* frHandler);
	void Unload();
	void Update(float totalTime, float deltaTime);
	void Render(float totalTime);
private:
	void rocketCrash();
	void procedurallyGenerateGround();
	void checkRocketGroundCollision(vector<RocketStage*> rocketStages);
	void rocketGroundCollision(RocketStage* stage);

	deque<CollisionSprite*> _ground;
	HUD* _hud;
	Sprite* _groundBackground;
	Sprite* _groundSecondBackground;
	Sprite* _oceanBackground;
	Sprite* _moonBackground;
	Sprite* _levelTransition;
	Text* _startGameText;
	CollisionSprite* _launchPad[2];
	Cloud* _clouds[2];

	bool _spacePressed = false;
	bool _spaceInQueue = false;
	bool _gameStarted = false;
	bool _crashed = false;
	const int _groundScalePercent = 200;
	float _GFS; //Gravitiational Field Strength
};