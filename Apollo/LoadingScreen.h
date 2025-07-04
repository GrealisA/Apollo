#pragma once
#include "Level.h"

class LoadingScreen : public Level {
	using Level::Level;
public:
	void Load() override;
	void Unload() override;
	void Update(float totalTime, float deltaTime) override;
	void Render(float totalTime) override;
private:
	Sprite* _loadingCursor;
};