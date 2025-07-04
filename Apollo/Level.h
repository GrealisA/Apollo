#pragma once
#include "FrameRateHandler.h"
#include "Rocket.h"
#include "RocketSaveState.h"
#include <conio.h>
#include <math.h>
#include <mmsystem.h>
#include <Windows.h>
#include <sys\stat.h>
#include <vector>
#include <thread>


using namespace std;

class Level{
public:
	explicit Level();
	virtual void Load(Graphics* gfx, FrameRateHandler* frHandler);
	virtual void Unload();
	virtual void Update(float totalTime, float deltaTime);
	virtual void Render(float totalTime);
	static int getLevelNo() { return _levelNo; }
	static void setLevelNo(int newLevelNo) { _levelNo = newLevelNo; _levelChanged = true;}
	static bool& getLevelChanged(){ return _levelChanged; }	

	void UpdateFrame();
protected:
	static void truncateDecimalPlaces(float &number, int decimalPlaces);
	static Sprite* createSprite(string fileDirectory, string imageFileName);
	static CollisionSprite* createCollisionSprite(string fileDirectory, string imageFileName, float scalePercent, float mass);
	static Rocket* createRocket(int rocketNo, float startingAltitude);
	static void checkCollisions(CollisionSprite* primaryCollisionSprite, CollisionSprite* secondaryCollisionSprite);
	static float calculateMoonGFS() { return (gravitionalConstant * moonMass) / pow(earthRadius + (maxAltitudeFromGround - _rocket->getRocketPhysics().displacement.vertical), 2); }
	static float calculateEarthGFS() { return (gravitionalConstant * earthMass) / pow(earthRadius + _rocket->getRocketPhysics().displacement.vertical, 2); }


	static Graphics* _gfx;
	static FrameRateHandler* _frHandler;
	static int _frame;
	static int _respawnTimer;
	static int _levelNo;
	static Rocket* _rocket;
	static RocketSaveState _rocketRespawnState;
	static bool _levelChanged;

	static queue<string> _keyPressed;
	static Sound* _backgroundMusic;
};