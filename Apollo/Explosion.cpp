#include "Explosion.h"
bool Explosion::_explosionOccuring;

Explosion::Explosion(int explosionTypeNo, PointXY crashLocation) : SpriteSheet("SpriteSheet\\Flames\\Explosion" + to_string(explosionTypeNo) + ".png", "Explosion" + to_string(explosionTypeNo), 500, 500) {
	_currentFrame = 0;
	_originalLocation = crashLocation;		
	//Plays the explosion sound effect if no other explosion is occurring at that tie
	if (!_explosionOccuring) _explosionSoundFX = new Sound("\\Sound\\Explosion.wav", -sqrt(pow(50 - crashLocation.x, 2) + pow(crashLocation.y - 50, 2)) * 10 , false);
	
	//Creates a randomly sized explosion spritesheet and matches the centre of the object with the centre of the explosion
	setScalePercent(float(rand() % 10) + 20);
	_sSettings.locationPercent.x = _originalLocation.x - (_sSettings.spriteDimensionsPercent.sizeX / 2);
	_sSettings.locationPercent.y = _originalLocation.y - (_sSettings.spriteDimensionsPercent.sizeY / 2);
	_explosionOccuring = true;
}

void Explosion::Render() {
	//Only renders the explosion if the cycle of the animation hasn't been completed already
	if (_currentFrame < 28) {
		//Ensures that rocket stages which have crashed don't cause audio distortion due to overlapping of audio
		if (_currentFrame > 3) _explosionOccuring = false;

		SpriteSheet::Render(int(trunc(_currentFrame / 4)));
		_currentFrame++;
	}
}