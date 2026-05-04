#pragma once
#include "GameObject.h"

#define PIRANHA_ARISE_SPEED 0.05f
#define PIRANHA_WAIT_TIME 2000
#define PIRANHA_TOP_WAIT 500

#define PIRANHA_BBOX_WIDTH 15
#define PIRANHA_BBOX_HEIGHT 19
#define PIRANHA_HEIGHT 17

#define ID_ANI_PIRANHA 14016

class PiranhaPlant : public GameObject
{
protected:
	ULONGLONG wait_start = 0;
	ULONGLONG top_start = 0;
	bool isWaiting = false;
	float originalY;
	bool isGoingUp = true;
	bool isAtTop = false;

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

	virtual int IsCollidable() { return 1; };
	virtual int IsBlocking() { return 0; }
	virtual void OnNoCollision(DWORD dt);

	virtual void OnCollisionWith(LPCOLLISIONEVENT e);

public:
	PiranhaPlant(float x, float y) : GameObject(x, y) {
		originalY = y;
	};
	bool GetIsWaiting() { return y == originalY; };
};