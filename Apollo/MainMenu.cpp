#include "MainMenu.h"

void MainMenu::Load(Graphics* gfx, FrameRateHandler* frHandler) {
	_gfx = gfx;
	_frHandler = frHandler;
	//Reference: http://forums.codeguru.com/showthread.php?500867-RESOLVED-How-do-I-remove-the-window-border-to-get-fullscreen
	_nonFullscreenStyle = GetWindowLong(_gfx->getHwnd(), GWL_STYLE);
	_nonFullscreenExStyle = GetWindowLong(_gfx->getHwnd(), GWL_EXSTYLE);
	//End Reference

	//Initialisation of sprites
	float tileYLocation = 90.0f;
	_menuChoice = 1;
	_settingsMenuOn = false;
	_selectedResolution.sizeX = _gfx->getRenderTarget()->GetSize().width;
	_selectedResolution.sizeY = _gfx->getRenderTarget()->GetSize().height;
	_selectedFrameRate = _frHandler->getFrameRate();

	_logo = createSprite("MainMenu", "Logo");
	_logo->setScalePercent(50);
	_logo->getSpriteSettings().locationPercent.x = 10.0f;
	_logo->getSpriteSettings().locationPercent.y = 10.0f;

	_astronaut = createSprite("MainMenu", "Astronaut");
	_astronaut->setScalePercent(100);
	_astronaut->getSpriteSettings().locationPercent.x = 0;
	_astronaut->getSpriteSettings().locationPercent.y = 65.0f;

	_moon = createSprite("LevelEarth", "Moon");
	_moon->setScalePercent(20);
	_moon->getSpriteSettings().locationPercent.x = 75.0f;
	_moon->getSpriteSettings().locationPercent.y = 10.0f;

	_stars = createSprite("MainMenu", "Stars");	
	_stars->setScalePercent(100);
	_stars->getSpriteSettings().locationPercent.x = 0;
	_stars->getSpriteSettings().locationPercent.y = 0;

	_cursor = createSprite("MainMenu", "Cursor");
	_cursor->setScalePercent(20);
	_cursor->getSpriteSettings().locationPercent.x = 10.0f;
	_cursor->getSpriteSettings().locationPercent.y = tileYLocation;

	_settingsMenuBackdrop = createSprite("MainMenu", "TextTile");
	_settingsMenuBackdrop->getSpriteSettings().locationPercent.x = 30.0f;
	_settingsMenuBackdrop->getSpriteSettings().locationPercent.y = 30.0f;
	_settingsMenuBackdrop->getSpriteSettings().spriteDimensionsPercent.sizeX = 40.0f;
	_settingsMenuBackdrop->getSpriteSettings().spriteDimensionsPercent.sizeY = 40.0f;
	_settingsMenuBackdrop->getSpriteSettings().opacity = 0.5f;

	//Initialisation of the main menu tiles
	for (int tileCount = 0; tileCount < size(_textTiles); tileCount++) {
		float xLocation;
		_textTiles[tileCount] = createSprite("MainMenu", "TextTile");		
		_textTiles[tileCount]->setScalePercent(20);

		switch (tileCount) {
		case 0:
			xLocation = 10;
			break;
		case 1:
			xLocation = 40;
			break;
		case 2:
			xLocation = 70;
			break;
		}
		_textTiles[tileCount]->getSpriteSettings().locationPercent.x = xLocation;
		_textTiles[tileCount]->getSpriteSettings().locationPercent.y = tileYLocation;
	}

	//Initialisation of text for the main menu
	_playText = setupText("Play", 8.0f);
	_playText->getTSettings().locationPercent.x = 10.0f;
	_playText->getTSettings().locationPercent.y = tileYLocation;
	_playText->getTSettings().textDimensionsPercent.sizeX = 20.0f;
	_playText->getTSettings().textDimensionsPercent.sizeY = _textTiles[0]->getSpriteSettings().spriteDimensionsPercent.sizeY;
	_playText->getTSettings().alignment = "Middle";

	_settingsText = setupText("Settings", 8.0f);
	_settingsText->getTSettings().locationPercent.x = 40.0f;
	_settingsText->getTSettings().locationPercent.y = tileYLocation;
	_settingsText->getTSettings().textDimensionsPercent.sizeX = 20.0f;
	_settingsText->getTSettings().textDimensionsPercent.sizeY = _textTiles[1]->getSpriteSettings().spriteDimensionsPercent.sizeY;
	_settingsText->getTSettings().alignment = "Middle";

	_quitText = setupText("Quit", 8.0f);
	_quitText->getTSettings().locationPercent.x = 70.0f;
	_quitText->getTSettings().locationPercent.y = tileYLocation;
	_quitText->getTSettings().textDimensionsPercent.sizeX = 20.0f;
	_quitText->getTSettings().textDimensionsPercent.sizeY = _textTiles[2]->getSpriteSettings().spriteDimensionsPercent.sizeY;
	_quitText->getTSettings().alignment = "Middle";	
	
	_resolutionLabel = setupText("Resolution: ", 8.0f);
	_resolutionLabel->getTSettings().locationPercent.x = 32.0f;
	_resolutionLabel->getTSettings().locationPercent.y = 35.0f;
	_resolutionLabel->getTSettings().textDimensionsPercent.sizeX = 20.0f;
	_resolutionLabel->getTSettings().textDimensionsPercent.sizeY = 5.0f;
	_resolutionLabel->getTSettings().alignment = "Middle";

	_currentResolutionLabel = setupText(to_string(int(_selectedResolution.sizeX)) + "x" + to_string(int(_selectedResolution.sizeY)), 8.0f);
	_currentResolutionLabel->getTSettings().locationPercent.x = 52.0f;
	_currentResolutionLabel->getTSettings().locationPercent.y = 35.0f;
	_currentResolutionLabel->getTSettings().textDimensionsPercent.sizeX = 20.0f;
	_currentResolutionLabel->getTSettings().textDimensionsPercent.sizeY = 5.0f;
	_currentResolutionLabel->getTSettings().alignment = "Middle";

	_fullScreenLabel = setupText("FullScreen: ", 8.0f);
	_fullScreenLabel->getTSettings().locationPercent.x = 32.0f;
	_fullScreenLabel->getTSettings().locationPercent.y = 45.0f;
	_fullScreenLabel->getTSettings().textDimensionsPercent.sizeX = 20.0f;
	_fullScreenLabel->getTSettings().textDimensionsPercent.sizeY = 5.0f;
	_fullScreenLabel->getTSettings().alignment = "Middle";

	_currentFullScreenLabel = setupText("Off", 8.0f);
	_currentFullScreenLabel->getTSettings().locationPercent.x = 52.0f;
	_currentFullScreenLabel->getTSettings().locationPercent.y = 45.0f;
	_currentFullScreenLabel->getTSettings().textDimensionsPercent.sizeX = 20.0f;
	_currentFullScreenLabel->getTSettings().textDimensionsPercent.sizeY = 5.0f;
	_currentFullScreenLabel->getTSettings().alignment = "Middle";

	_frameRateLabel = setupText("Frame Rate: ", 8.0f);
	_frameRateLabel->getTSettings().locationPercent.x = 32.0f;
	_frameRateLabel->getTSettings().locationPercent.y = 55.0f;
	_frameRateLabel->getTSettings().textDimensionsPercent.sizeX = 20.0f;
	_frameRateLabel->getTSettings().textDimensionsPercent.sizeY = 5.0f;
	_frameRateLabel->getTSettings().alignment = "Middle";

	_currentFrameRateLabel = setupText(to_string(_selectedFrameRate), 8.0f);
	_currentFrameRateLabel->getTSettings().locationPercent.x = 52.0f;
	_currentFrameRateLabel->getTSettings().locationPercent.y = 55.0f;
	_currentFrameRateLabel->getTSettings().textDimensionsPercent.sizeX = 20.0f;
	_currentFrameRateLabel->getTSettings().textDimensionsPercent.sizeY = 5.0f;
	_currentFrameRateLabel->getTSettings().alignment = "Middle";

	_returnToMainMenuLabel = setupText("Apply & Return", 8.0f);
	_returnToMainMenuLabel->getTSettings().locationPercent.x = 40.0f;
	_returnToMainMenuLabel->getTSettings().locationPercent.y = 65.0f;
	_returnToMainMenuLabel->getTSettings().textDimensionsPercent.sizeX = 20.0f;
	_returnToMainMenuLabel->getTSettings().textDimensionsPercent.sizeY = 5.0f;
	_returnToMainMenuLabel->getTSettings().alignment = "Middle";
}

void MainMenu::Unload() {

}

void MainMenu::Update(float totalTime, float deltaTime) {
	UpdateFrame();
	//Resets the last key pressed when no key has been pressed
	if (_cursorLast == "Left" && _keyPressed.empty()) _cursorLast = "";
	if (_cursorLast == "Right" && _keyPressed.empty()) _cursorLast = "";
	if (_cursorLast == "Up" && _keyPressed.empty()) _cursorLast = "";
	if (_cursorLast == "Down" && _keyPressed.empty()) _cursorLast = "";
	if (_cursorLast == "Enter" && _keyPressed.empty()) _cursorLast = "";

	//Checks for any key presses that will change the selected choice
	while (!_keyPressed.empty()) {
		if (_keyPressed.front() == "Left" && _cursorLast != _keyPressed.front()) {
			//In the main menu, the left key moves the cursor left unless at the left most choice where it goes to the right most choice
			if (!_settingsMenuOn) {
				if (_menuChoice > 1) {
					_menuChoice -= 1;
					_cursor->getSpriteSettings().locationPercent.x -= 30.0f;
				}
				else {
					_menuChoice = 3;
					_cursor->getSpriteSettings().locationPercent.x = 70.0f;
				}
			}
			//In the settings menu, this cycles through the possible choices available for the selected settings
			else if (_cursor->getSpriteSettings().locationPercent.y == _resolutionLabel->getTSettings().locationPercent.y) {
				//Decreases the resolution setting
				if (_selectedResolution.sizeX > 1280) _selectedResolution.sizeX /= 1.5f;
				_selectedResolution.sizeY = _selectedResolution.sizeX * 0.5625f;
				_currentResolutionLabel->getTSettings().text = to_string(int(_selectedResolution.sizeX)) + "x" + to_string(int(_selectedResolution.sizeY));
				_resolutionChange = true;
			}
			else if (_cursor->getSpriteSettings().locationPercent.y == _fullScreenLabel->getTSettings().locationPercent.y) {
				//Toggles the fullscreen setting
				_fullscreenMode = !_fullscreenMode;
				if (_fullscreenMode) _currentFullScreenLabel->getTSettings().text = "On";
				else _currentFullScreenLabel->getTSettings().text = "Off";
				_fullScreenChange = true;
			}
			else if (_cursor->getSpriteSettings().locationPercent.y == _frameRateLabel->getTSettings().locationPercent.y && _selectedFrameRate == 60) {
				//Decreases the frame rate setting
				_selectedFrameRate = 24;
				_currentFrameRateLabel->getTSettings().text = to_string(_selectedFrameRate);
				_frameRateChange = true;
			}
			_cursorLast = _keyPressed.front();
		}
		else if (_keyPressed.front() == "Right" && _cursorLast != _keyPressed.front()) {
			//In the main menu, the right key moves the cursor right unless at the right most choice where it goes to the left most choice
			if (!_settingsMenuOn) {
				if (_menuChoice < 3) {
					_menuChoice += 1;
					_cursor->getSpriteSettings().locationPercent.x += 30.0f;
				}
				else {
					_menuChoice = 1;
					_cursor->getSpriteSettings().locationPercent.x = 10.0f;
				}
			}
			//In the settings menu, this cycles through the possible choices available for the selected settings
			else if (_cursor->getSpriteSettings().locationPercent.y == _resolutionLabel->getTSettings().locationPercent.y) {
				//Increases the resolution setting
				if (_selectedResolution.sizeX < 5000) _selectedResolution.sizeX *= 1.5f;
				_selectedResolution.sizeY = _selectedResolution.sizeX * 0.5625f;
				_currentResolutionLabel->getTSettings().text = to_string(int(_selectedResolution.sizeX)) + "x" + to_string(int(_selectedResolution.sizeY));
				_resolutionChange = true;
			}
			else if (_cursor->getSpriteSettings().locationPercent.y == _fullScreenLabel->getTSettings().locationPercent.y) {
				//Toggles the fullscreen setting
				_fullscreenMode = !_fullscreenMode;
				if (_fullscreenMode) _currentFullScreenLabel->getTSettings().text = "On";
				else  _currentFullScreenLabel->getTSettings().text = "Off";
				_fullScreenChange = true;
			}
			else if (_cursor->getSpriteSettings().locationPercent.y == _frameRateLabel->getTSettings().locationPercent.y && _selectedFrameRate == 24) {
				//Increases the frame rate setting
				_selectedFrameRate = 60;
				_currentFrameRateLabel->getTSettings().text = to_string(_selectedFrameRate);
				_frameRateChange = true;
			}
			_cursorLast = _keyPressed.front();
		}
		else if (_keyPressed.front() == "Up" && _cursorLast != _keyPressed.front()) {
			//If in the settings menu, he up key moves the cursor over options unless at the top where it selects the return to menu option
			if (_settingsMenuOn) {
				if (_settingsMenuChoice > 0) {
					_settingsMenuChoice -= 1;
					_cursor->getSpriteSettings().locationPercent.y -= 10.0f;
				}
				else {
					_settingsMenuChoice = 3;
				}					
				if(_settingsMenuChoice == 3) _cursor->getSpriteSettings().locationPercent = _returnToMainMenuLabel->getTSettings().locationPercent;
				else _cursor->getSpriteSettings().locationPercent.x = _resolutionLabel->getTSettings().locationPercent.x;
			}
			_cursorLast = _keyPressed.front();
		}
		else if (_keyPressed.front() == "Down" && _cursorLast != _keyPressed.front()) {
			//If in the settings menu, he down key moves the cursor over options unless at the return to menu option where it moves the cursor to the top of the settings menu
			if (_settingsMenuOn) {
				if (_settingsMenuChoice < 3) {
					_settingsMenuChoice += 1;
					_cursor->getSpriteSettings().locationPercent.y += 10.0f;
				}
				else {
					_settingsMenuChoice = 0;
					_cursor->getSpriteSettings().locationPercent = _resolutionLabel->getTSettings().locationPercent;
				}
				if (_settingsMenuChoice == 3) _cursor->getSpriteSettings().locationPercent = _returnToMainMenuLabel->getTSettings().locationPercent;
				else _cursor->getSpriteSettings().locationPercent.x = _resolutionLabel->getTSettings().locationPercent.x;
			}
			_cursorLast = _keyPressed.front();
		}
		else if (_keyPressed.front() == "Enter"  && _cursorLast != _keyPressed.front()) {
			_cursorLast = _keyPressed.front();
			if (!_settingsMenuOn) {
				//If in the main menu, pressing the enter key causes the current choice to be loaded
				if (_menuChoice != 2) setLevelNo(_menuChoice);
				else {
					_settingsMenuOn = true;
					_cursor->getSpriteSettings().locationPercent = _resolutionLabel->getTSettings().locationPercent;
				}
			}
			else if (_settingsMenuChoice == 3) {
					//If in the settings menu and above the return to main menu option, all settings are applied to the program
					_settingsMenuOn = false;
					_cursor->getSpriteSettings().locationPercent = _settingsText->getTSettings().locationPercent;
					if (_resolutionChange) {
						//This causes the program to change the resolution of the graphics window
						_gfx->getRenderTarget()->Resize(D2D1::SizeU(int(_selectedResolution.sizeX), int(_selectedResolution.sizeY)));
						_gfx->getWindowSize() = _selectedResolution;
					}
					if (_fullScreenChange) {
						//This causes the program to toggle between fullscreen and windowed mode depending on what is chosen by the user
						if (_fullscreenMode) {
							//Reference: http://forums.codeguru.com/showthread.php?500867-RESOLVED-How-do-I-remove-the-window-border-to-get-fullscreen
							LONG style = GetWindowLong(_gfx->getHwnd(), GWL_STYLE);
							_nonFullscreenStyle = style;
							style &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU);
							SetWindowLong(_gfx->getHwnd(), GWL_STYLE, style);
							LONG exStyle = GetWindowLong(_gfx->getHwnd(), GWL_EXSTYLE);
							_nonFullscreenExStyle = exStyle;
							exStyle &= ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
							SetWindowLong(_gfx->getHwnd(), GWL_EXSTYLE, exStyle);
							SetWindowPos(_gfx->getHwnd(), HWND_TOP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_FRAMECHANGED | SWP_SHOWWINDOW);
						}
						else {
							SetWindowLong(_gfx->getHwnd(), GWL_STYLE, _nonFullscreenStyle);
							SetWindowLong(_gfx->getHwnd(), GWL_EXSTYLE, _nonFullscreenExStyle);
							SetWindowPos(_gfx->getHwnd(), HWND_NOTOPMOST, (GetSystemMetrics(SM_CXSCREEN) - LONG(_gfx->getWindowSize().sizeX)) / 2, (GetSystemMetrics(SM_CYSCREEN) - LONG(_gfx->getWindowSize().sizeY)) / 2, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
							//End Reference
						}
					}
					//This causes the program to change the frame rate of the graphics window
					if (_frameRateChange) _frHandler = new FrameRateHandler(_selectedFrameRate);
				}
		}
		_keyPressed.pop();
	}
}

void MainMenu::Render(float time) {
	_gfx->ClearScreen(0, 0, 0, 1);
	//Renders all sprites in the main menu
	_stars->Render();
	_moon->Render();
	_astronaut->Render();
	_logo->Render();
	for each(Sprite* tile in _textTiles) tile->Render();
	_playText->Render();
	_settingsText->Render();
	_quitText->Render();
	if (_settingsMenuOn) {
		_settingsMenuBackdrop->Render();		
		_resolutionLabel->Render();
		_currentResolutionLabel->Render();
		_fullScreenLabel->Render();
		_currentFullScreenLabel->Render();
		_frameRateLabel->Render();
		_currentFrameRateLabel->Render();
		_returnToMainMenuLabel->Render();
	}
	_cursor->Render();
}

//Initialises and returns text specified by the parameters
Text* MainMenu::setupText(string text, float fontSize) {
	Text* newText = new Text(L"Consolas", fontSize, _gfx);
	newText->getTSettings().text = text;
	newText->getTSettings().colour = D2D1::ColorF::AliceBlue;
	return newText;
}