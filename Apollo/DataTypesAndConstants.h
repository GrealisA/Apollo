#pragma once

using namespace std;

//Contains all data structures and constants
struct SizeXY {
	float sizeX, sizeY;
};

struct PointXY {
	float x, y;
};

struct Colour {
	float r, g, b, a;
};

struct VectorQuantity {
	float general, vertical, horizontal;
};

struct PhysicsVector {
	VectorQuantity displacement, force, velocity, acceleration;
	PointXY CentreOfRotation;
	float mass, angularVelocity;
};

const float pi = 3.141592f;
const float gravitionalConstant = 6.67f * float(pow(10, -11));
const float earthMass = 5.97f * float(pow(10, 24));
const float earthRadius = 6.37f * float(pow(10, 6));
const float moonMass = 7.35f * float(pow(10, 22));
const float moonRadius = 1.74f * float(pow(10, 6));
const float groundMu = 0.79f;
const int karmanLine = int(pow(10,5));
const int maxHorizontalDisplacement = 4000;
const int maxAltitudeFromGround = 485000;
const bool collisionBoxMode = false;