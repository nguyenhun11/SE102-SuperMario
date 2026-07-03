#pragma once
#include "GameObject.h"

#define BOOMERANG_SPEED_X 0.1f
#define BOOMERANG_GRAVITY 0.00001f    
#define BOOMERANG_MAX_DISTANCE 120.0f // Quãg đường bay = 8 ô
#define BOOMERANG_MAX_HEIGHT   24.0f  // Độ cao Parabol muốn đạt tới
#define BOOMERANG_RETURN_TIME 1200.0f // Thời gian bay về
#define BOOMERANG_DELAY_TIME 200.0f   // Thời gian trễ trước khi bay

#define ID_ANI_BOOMERANG 20402
#define ID_ANI_BOOMERANG_IDLE 2040201

class BoomerangBro;

class Boomerang : public GameObject
{
protected:
	float startX;
	float ay;

	bool isReturning;
	float returnTimer;
	BoomerangBro* owner;

	bool isDelayed;
	float delayTimer;

public:
	Boomerang(float x, float y, int nx, BoomerangBro* owner);
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

	virtual int IsCollidable() { return 1; }
	virtual int IsBlocking() { return 0; }
};