#pragma once

#include "Level.h"
#include "Rocket.h"
#include "Satellite.h"
#include "HUD.h"

class LevelSpace : public Level {
	using Level::Level;
public:
	void Load(Graphics* gfx, FrameRateHandler* frHandler);
	void Unload();
	void Update(float totalTime, float deltaTime);
	void Render(float totalTime);
private:
	void generateStars();
	
	HUD* _hud;
	Sprite* _earthBackground;
	Sprite* _moonBackground;
	deque<Sprite*> _starsBackground;
	Satellite* _sputnik;

	bool _spacePressed = false;
	bool _spaceInQueue = false;
	float _GFS; //Gravitiational Field Strength
};
