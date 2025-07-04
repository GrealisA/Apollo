#pragma once

#include "Level.h"
#include "Rocket.h"
#include "HUD.h"

class LevelMoon : public Level {
	using Level::Level;
public:
	void Load(Graphics* gfx, FrameRateHandler* frHandler);
	void Unload();
	void Update(float totalTime, float deltaTime);
	void Render(float totalTime);
private:
	void procedurallyGenerateGround();
	void checkRocketGroundCollision(vector<RocketStage*> rocketStages);
	void rocketGroundCollision(RocketStage* stage);

	deque<CollisionSprite*> _ground;
	HUD* _hud;
	Sprite* _moonBackground;
	Sprite* _earth;
	Sprite* _stars;
	Sprite* _victoryScreen;

	bool _spacePressed = false;
	bool _spaceInQueue = false;
	bool _crashed = false;
	bool _endScreen = false;
	const int _groundScalePercent = 200;
	float _GFS; //Gravitiational Field Strength
};