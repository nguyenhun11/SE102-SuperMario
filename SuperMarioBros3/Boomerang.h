#pragma once
#include "GameObject.h"

#define BOOMERANG_SPEED_X 0.15f
#define BOOMERANG_GRAVITY 0.0001f    
#define BOOMERANG_MAX_DISTANCE 128.0f // Quãg đường bay = 8 ô
#define BOOMERANG_RETURN_TIME 1200.0f // Thời gian bay về

#define ID_ANI_BOOMERANG 20402

class BoomerangBro;

class Boomerang : public GameObject
{
protected:
	float startX;
	float ay;

	bool isReturning;
	float returnTimer;
	BoomerangBro* owner;

public:
	Boomerang(float x, float y, int nx, BoomerangBro* owner);
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

	virtual int IsCollidable() { return 1; }
	virtual int IsBlocking() { return 0; }
};