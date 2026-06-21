#pragma once
#include "GameObject.h"
#include "RespawnableEnemy.h"
#include "Animations.h"
#include "Animation.h"
#include "Sensor.h"

#define KOOPA_GRAVITY 0.002f
#define KOOPA_WALKING_SPEED 0.02f
#define KOOPA_SHELL_SPEED 0.22f

#define KOOPA_BBOX_OFFSET_Y 4

#define KOOPA_BBOX_WIDTH 15

#define KOOPA_BBOX_HEIGHT 24
#define KOOPA_BBOX_HEIGHT_DIE 14

#define KOOPA_DIE_TIMEOUT 6000

// GREEN
#define ID_ANI_KOOPA_GREEN_WALKING 20210
//#define ID_ANI_KOOPA_DIE 20220
#define ID_ANI_KOOPA_GREEN_SHELL 20211
#define ID_ANI_KOOPA_GREEN_SHELL_SHAKING 20212
#define ID_ANI_KOOPA_GREEN_SHELL_MOVING 20213
#define ID_ANI_KOOPA_GREEN_WING 20214

// RED
#define ID_ANI_KOOPA_RED_WALKING 21220
//#define ID_ANI_KOOPA_DIE 20220
#define ID_ANI_KOOPA_RED_SHELL 21221
#define ID_ANI_KOOPA_RED_SHELL_SHAKING 21222
#define ID_ANI_KOOPA_RED_SHELL_MOVING 21223
#define ID_ANI_KOOPA_RED_WING 21224

enum class KoopaState
{
	WALKING = 0,
	SHELL = 1,
	SHELL_MOVING = 2,
	WING = 3,
	SHAKING = 4,
	SHELL_UPWARD = 5,
	DIE = 99
};

enum class KoopaColor
{
	GREEN = 0,
	RED = 1
};

class Koopa : public RespawnableEnemy
{
protected:
	//float ax;
	//float ay;
	bool isFlipped; // Biến cờ để theo dõi trạng thái lật của sprite (điều khiển hướng render)
	bool isFlippedVertical;
	Sensor* sensorfront;
	Sensor* sensorback;

	KoopaColor color;
	
	
	ULONGLONG die_start;

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

	//virtual int IsCollidable() { return 1; };
	//virtual int IsBlocking() { return 0; }
	int IsBlocking() override
	{
		if (state == static_cast<int>(KoopaState::DIE) || isHeld) return 0;
		return 1;
	}

	int IsCollidable() override
	{
		if (state == static_cast<int>(KoopaState::DIE) || isHeld) return 0;
		return 1;
	}
	virtual void OnNoCollision(DWORD dt);

	virtual void OnCollisionWith(LPCOLLISIONEVENT e);
	virtual void OnCollisionWithBrick(LPCOLLISIONEVENT e);
	virtual void OnCollisionWithQuestionBlock(LPCOLLISIONEVENT e);
	virtual void OnCollisionWithGoomba(LPCOLLISIONEVENT e);
	virtual void OnCollisionWithKoopa(LPCOLLISIONEVENT e);

public:
	bool isHeld = false;
	Koopa(float x, float y, KoopaColor color = KoopaColor::GREEN);
	virtual void SetState(KoopaState state);
	void SetNx(float nx) { this->nx = nx; };
	void ChangeDirection()
	{
		vx = -vx;
		if (vx > 0) nx = 1;
		else nx = -1;
	}

	virtual void OnEnable() override;
	virtual void OnExitCamera() override;

	void SetDirection(int d);

};




