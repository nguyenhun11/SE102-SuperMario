#pragma once
#include "Koopa.h"

#define ID_ANI_RED_KOOPA_WALKING 21220
#define ID_ANI_RED_KOOPA_SHELL 21221
#define ID_ANI_RED_KOOPA_SHELL_SHAKING 21222
#define ID_ANI_RED_KOOPA_SHELL_MOVING 21223

#define RED_KOOPA_BBOX_WIDTH 15
#define RED_KOOPA_BBOX_HEIGHT 24

#define RED_KOOPA_BBOX_HEIGHT_DIE 14


class RedKoopa : public Koopa
{
protected:
	float ax;
	float ay;
	Sensor* sensorfront;
	Sensor* sensorback;


	ULONGLONG die_start;

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

	virtual int IsCollidable() { return 1; };
	virtual int IsBlocking() { return 0; }
	virtual void OnNoCollision(DWORD dt);

	virtual void OnCollisionWith(LPCOLLISIONEVENT e);

public:
	RedKoopa(float x, float y);
	virtual void SetState(KoopaState state);
	void ChangeDirection()
	{
		vx = -vx;
		if (vx > 0) nx = 1;
		else nx = -1;
	}
};


