#pragma once
#include "GameObject.h"
#include "Animations.h"
#include "Animation.h"

#define KOOPA_GRAVITY 0.002f
#define KOOPA_WALKING_SPEED 0.02f


#define KOOPA_BBOX_WIDTH 15

#define KOOPA_BBOX_HEIGHT 24
#define KOOPA_BBOX_HEIGHT_DIE 14

#define KOOPA_DIE_TIMEOUT 500

#define KOOPA_STATE_WALKING 100
#define KOOPA_STATE_DIE 200
#define KOOPA_STATE_SHELL 300
#define KOOPA_STATE_SHELL_MOVING 400
#define KOOPA_STATE_WING 500

#define ID_ANI_KOOPA_WALKING 20210
#define ID_ANI_KOOPA_DIE 20220
#define ID_ANI_KOOPA_SHELL 20230

struct SensorData
{
	float x;
	float y;
	float width;
	float height;
	SensorData(float x = 0, float y = 0, float width = 0, float height = 0) : x(x), y(y), width(width), height(height) {}
	bool IsNonCollidingWith(LPCOLLISIONEVENT e)
	{
		float left = x - width / 2;
		float top = y - height / 2;
		float right = left + width;
		float bottom = top + height;
		float objLeft, objTop, objRight, objBottom;
		e->obj->GetBoundingBox(objLeft, objTop, objRight, objBottom);
		return (right <= objLeft || left >= objRight || bottom <= objTop || top >= objBottom);
	}

};

class Koopa : public GameObject
{
protected:
	float ax;
	float ay;
	SensorData* sensorfront;
	SensorData* sensorback;
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
	virtual void SetState(int state);
};





