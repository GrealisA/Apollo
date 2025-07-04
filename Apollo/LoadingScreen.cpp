#include "LoadingScreen.h"


void LoadingScreen::Load() {
	LoadingScreen::NoOfSprites = 1;
	LoadingScreen::NoOfSpriteSheets = 0;
	LoadingScreen::_spriteFilepath = "Sprites\\LoadingScreen\\";
	LoadingScreen::_spriteSheetFilepath = "SpriteSheet\\LoadingScreen\\";
	LoadAllSprites(LoadingScreen::NoOfSprites, LoadingScreen::NoOfSpriteSheets);

	_loadingCursor = _sprites[0];
	_loadingCursor->Settings.x = 125;	_loadingCursor->Settings.y = 500;	_loadingCursor->Settings.opacity = 1;	_loadingCursor->Settings.xScale = 0.2f;		_loadingCursor->Settings.yScale = 0.2f;	
	_loadingCursor->Settings.xCentreOfMass = (_loadingCursor->getBMPsize().width*_loadingCursor->Settings.xScale) / 2; _loadingCursor->Settings.yCentreOfMass = (_loadingCursor->getBMPsize().height*_loadingCursor->Settings.yScale) / 2;

	switch (switchLevelNo) {
		case(1):
			setSwitchLevelNo(/*5*/6);
		case(2):
			setSwitchLevelNo(7);
	}
}
void LoadingScreen::Unload() {
	UnloadSprites();
}
void LoadingScreen::Update(float totalTime, float deltaTime) {
	UpdateFrame();

	_loadingCursor->Rotate(360*deltaTime, _loadingCursor->Settings.x + _loadingCursor->Settings.xCentreOfMass, _loadingCursor->Settings.y + _loadingCursor->Settings.yCentreOfMass);
}
void LoadingScreen::Render(float totalTime) {
	gfx->ClearScreen(183, 210, 255, 1);
	_loadingCursor->Render();
}