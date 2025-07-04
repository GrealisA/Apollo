#include "Level.h"
#include "DataTypesAndConstants.h"

int Level::_frame;
int Level::_respawnTimer;
int Level::_levelNo = 1;
bool Level::_levelChanged;
Graphics* Level::_gfx;
FrameRateHandler* Level::_frHandler;
queue<string> Level::_keyPressed;
Sound* Level::_backgroundMusic;
Rocket* Level::_rocket;
RocketSaveState Level::_rocketRespawnState;

Level::Level() {
}

void Level::Load(Graphics* gfx, FrameRateHandler* frHandler) {}
void Level::Unload() {}
void Level::Update(float totalTime, float deltaTime) {}
void Level::Render(float totalTime) {}

void Level::UpdateFrame() {
	_frame++;
	//Keeps the frame bewteen 0 and 59
	_frame %= _frHandler->getFrameRate();

	//Checks if the window is selected then checks for key presses from the pre-defined keys
	if (GetFocus() == _gfx->getHwnd()) {
		if (GetAsyncKeyState(VK_LEFT) < 0) _keyPressed.push("Left");
		if (GetAsyncKeyState(VK_RIGHT) < 0) _keyPressed.push("Right");
		if (GetAsyncKeyState(VK_UP) < 0) _keyPressed.push("Up");
		if (GetAsyncKeyState(VK_DOWN) < 0) _keyPressed.push("Down");
		if (GetAsyncKeyState(VK_RCONTROL) < 0 || GetAsyncKeyState(VK_LCONTROL) < 0) _keyPressed.push("Control");
		if (GetAsyncKeyState(VK_RETURN) < 0) _keyPressed.push("Enter");
		if (GetAsyncKeyState(VK_SPACE) < 0) _keyPressed.push("Space");
		if (GetAsyncKeyState(VK_ESCAPE) < 0) _keyPressed.push("Escape");
	}
}

//Removes unneccessary decimal places from the specified variable
void Level::truncateDecimalPlaces(float &number, int decimalPlaces) {
	int temp = int(trunc(number * pow(10,decimalPlaces)));
	number = temp / float(pow(10, decimalPlaces));
}

//Initialises and returns sprites specified in the parameters
Sprite* Level::createSprite(string fileDirectory, string imageFileName) {
	Sprite* newSprite;
	newSprite = new Sprite("Sprites\\" + fileDirectory + "\\" + imageFileName + ".png", imageFileName);
	return newSprite;
}

//Initialises and returns collision sprites specified in the parameters
CollisionSprite* Level::createCollisionSprite(string fileDirectory, string imageFileName, float scalePercent, float mass) {
	CollisionSprite* newCollisionSprite;
	newCollisionSprite = new CollisionSprite("Sprites\\" + fileDirectory + "\\" + imageFileName + ".png", imageFileName);
	newCollisionSprite->setScalePercent(scalePercent);
	newCollisionSprite->getSpritePhysics().mass = mass;
	return newCollisionSprite;
}

//Initialises the rocket
Rocket* Level::createRocket(int rocketNo, float startingAltitude) {
	Rocket* newRocket = new Rocket(rocketNo, startingAltitude, _gfx, _frHandler);
	newRocket->getRocketPhysics().displacement.horizontal = newRocket->getRocketStartingLocation().x;
	for (int stageNo = 0; stageNo < newRocket->getRocketStages().size(); stageNo++)
		newRocket->getRocketStages()[stageNo]->getSpriteSettings().rotatedLocationPercent = newRocket->getRocketStages()[stageNo]->getSpriteSettings().locationPercent;
	return newRocket;
}

void Level::checkCollisions(CollisionSprite* primaryCollisionSprite, CollisionSprite* secondaryCollisionSprite) {
	CollisionBox* box = primaryCollisionSprite->getSpriteCollisionBox();
	CollisionBox* comparedBox = secondaryCollisionSprite->getSpriteCollisionBox();
	bool collisionBetweenBoxes = false;
	//Goes through each collision box point on the primary collision sprite and compares them with the secondary sprites collision box points
	for (int cornerNo = 0; cornerNo < 4; cornerNo++) {
		//If a point is within the area where the other collision box is, then the two are considered to be colliding
		if (box->getBoxCornerLocations()[cornerNo].x > comparedBox->getBoxCornerLocations()[0].x
			&& box->getBoxCornerLocations()[cornerNo].y > comparedBox->getBoxCornerLocations()[0].y
			&& box->getBoxCornerLocations()[cornerNo].x < comparedBox->getBoxCornerLocations()[3].x
			&& box->getBoxCornerLocations()[cornerNo].y < comparedBox->getBoxCornerLocations()[3].y)
		{
			box->getBoxCornerLocations()[cornerNo].colliding = true;
			collisionBetweenBoxes = true;
		}
		else box->getBoxCornerLocations()[cornerNo].colliding = false;
	}
	if (collisionBetweenBoxes) {
		//Pushes the two colliding sprites into the colliding sprites vector array
		CollisionSprite::getCollidingCollisionSprites().push(primaryCollisionSprite);
		CollisionSprite::getCollidingCollisionSprites().push(secondaryCollisionSprite);
	}
}