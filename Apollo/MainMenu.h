#pragma once

#include "Level.h"
#include "Text.h"
#include <wtypes.h>

class MainMenu: public Level
{
	using Level::Level;
public:
	void Load(Graphics* gfx, FrameRateHandler* frHandler) override;
	void Unload() override;
	void Update(float totalTime, float deltaTime) override;
	void Render(float totalTime) override;
	Text* setupText(string text, float fontSize);
private:
	int _menuChoice;
	int _settingsMenuChoice;
	int _selectedFrameRate;
	bool _settingsMenuOn;
	bool _fullscreenMode;
	bool _resolutionChange;
	bool _fullScreenChange;
	bool _frameRateChange;
	LONG _nonFullscreenStyle, _nonFullscreenExStyle;
	SizeXY _selectedResolution;
	string _cursorLast;
	Sprite* _logo;
	Sprite* _astronaut;
	Sprite* _moon;
	Sprite* _stars;	
	Sprite* _cursor;
	Sprite* _textTiles[3];
	Sprite* _settingsMenuBackdrop;
	
	Text* _playText;
	Text* _settingsText;
	Text* _quitText;
	Text* _resolutionLabel;
	Text* _currentResolutionLabel;
	Text* _fullScreenLabel;
	Text* _currentFullScreenLabel;
	Text* _frameRateLabel;
	Text* _currentFrameRateLabel;
	Text* _returnToMainMenuLabel;
};