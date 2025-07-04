#pragma once
#include "CollisionBox.h"
#include <queue>

class CollisionSprite : public Sprite
{
public:
	CollisionSprite(string filename, string spriteName);
	~CollisionSprite();
	void Update();

	void createRectangularCollisionBox();
	void Rotate(float angle, PointXY centrePoint);

	static collisionBoxPoint getRotatedPoint(collisionBoxPoint oldPoint, float angle, PointXY centrePoint);
	static queue<CollisionSprite*>& getCollidingCollisionSprites() { return _collidingCollisionSprites; }
	CollisionBox* getSpriteCollisionBox() { return _spriteCollisionBox; }
	PhysicsVector& getSpritePhysics() { return _spritePhysics; }

protected:	
	CollisionBox* _spriteCollisionBox;
	PhysicsVector _spritePhysics;
	static queue<CollisionSprite*> _collidingCollisionSprites;
};

