#include "LevelManager.h"

//Reference: https://www.youtube.com/watch?v=ZjGp9ILEiMc

LevelManager::LevelManager(Graphics* gfx) {
	_gfx = gfx;
	Sprite::setGFX(_gfx);
	_loading = false;
	_currentLevel = 0;
	_frTimer = new FrameRateHandler(60);
	_mainMenu = new MainMenu();
}

void LevelManager::LoadInitialLevel(Level *level) {
	_currentLevel = level;
	_loading = true;
	level->Load(_gfx, _frTimer);
	_loading = false;
}
void LevelManager::ChangeLevel(Level *level) {
	_loading = true;
	_currentLevel->Unload();
	level->Load(_gfx, _frTimer);
	delete _currentLevel;
	_currentLevel = level;
	_loading = false;
}

void LevelManager::RenderLevel() {
	if (_loading) return;
	_currentLevel->Render((float)_frTimer->GetTotalTime());
}

//End Reference

void LevelManager::UpdateLevel() {
	if (_loading) return;
	_frTimer->Update();
	_currentLevel->Update((float)_frTimer->GetTotalTime(), (float)_frTimer->GetDeltaTime());

	//Checks if a request to change level has been sent and switches the current level to the desired level
	if (Level::getLevelChanged()) {
		switch (Level::getLevelNo()) {
		case 0:
			//Loads the Main Menu
			_mainMenu = new MainMenu();
			ChangeLevel(_mainMenu);
			break;
		case 1:
			//Loads the Earth level
			_earthLevel = new LevelEarth();
			ChangeLevel(_earthLevel);
			break;
		case 3:
			//Exits the game
			exit(0);
			break;
		case 4:
			//Loads the Space level
			_spaceLevel = new LevelSpace();
			ChangeLevel(_spaceLevel);
			break;
		case 5:
			//Loads the Moon level
			_moonLevel = new LevelMoon();
			ChangeLevel(_moonLevel);
			break;
		default:
			break;
		}
		Level::getLevelChanged() = false;
	}
}



