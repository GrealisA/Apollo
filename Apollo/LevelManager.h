#pragma once

#include "MainMenu.h"
#include "LevelEarth.h"
#include "LevelSpace.h"
#include "LevelMoon.h"
#include <iostream>

using namespace std;

class LevelManager
{
public:
	//Reference: https://www.youtube.com/watch?v=ZjGp9ILEiMc
	LevelManager(Graphics* gfx);
	void LoadInitialLevel(Level *level);
	void ChangeLevel(Level *level);
	void RenderLevel();
	//End Reference

	void UpdateLevel();

	MainMenu* getMainMenu() { return _mainMenu; }

private:
	//Reference: https://www.youtube.com/watch?v=ZjGp9ILEiMc
	bool _loading;
	Level* _currentLevel;
	//End Reference

	FrameRateHandler* _frTimer;
	Graphics* _gfx;
	
	//Levels
	LevelEarth* _earthLevel;
	LevelSpace* _spaceLevel;
	LevelMoon* _moonLevel;
	MainMenu* _mainMenu;
};