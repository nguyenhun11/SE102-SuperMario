#pragma once
#include "GameObject.h"
#include "Animations.h"
#include "Animation.h"
#include "Sensor.h"

#define KOOPA_GRAVITY 0.002f
#define KOOPA_WALKING_SPEED 0.02f
#define KOOPA_SHELL_SPEED 0.1f


#define KOOPA_BBOX_WIDTH 15

#define KOOPA_BBOX_HEIGHT 24
#define KOOPA_BBOX_HEIGHT_DIE 14

#define KOOPA_DIE_TIMEOUT 2000
#define ID_ANI_KOOPA_WALKING 20210
//#define ID_ANI_KOOPA_DIE 20220
#define ID_ANI_KOOPA_SHELL 20211
#define ID_ANI_KOOPA_SHELL_SHAKING 20212
#define ID_ANI_KOOPA_SHELL_MOVING 20213


enum class KoopaState
{
	WALKING = 0,
	SHELL = 1,
	SHELL_MOVING = 2,
	WING = 3,
	SHAKING = 4,
	DIE = 99
};



class Koopa : public GameObject
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
	Koopa(float x, float y);
	virtual void SetState(KoopaState state);
	void ChangeDirection()
	{
		vx = -vx;
		if (vx > 0) nx = 1;
		else nx = -1;
	}
};




