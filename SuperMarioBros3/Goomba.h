#pragma once
#include "RespawnableEnemy.h"

#define GOOMBA_GRAVITY 0.001f
#define GOOMBA_WALKING_SPEED 0.03f

#define GOOMBA_BBOX_WIDTH 16
#define GOOMBA_BBOX_HEIGHT 14
#define GOOMBA_BBOX_HEIGHT_DIE 7

#define GOOMBA_DIE_TIMEOUT 500

#define ID_ANI_GOOMBA_WALKING 5000
#define ID_ANI_GOOMBA_DIE 5001

enum class GoombaState
{
	WALKING = 0,
	DIE = 1,
	BOUNCE = 2
};

class Goomba : public RespawnableEnemy
{
protected:
	bool isFlippedVertical = false;
	bool isOnSlope = false;
	ULONGLONG die_start;

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

	virtual int IsCollidable()
	{
		if (state == static_cast<int>(GoombaState::BOUNCE))
			return 0;
		return 1;
	}

	virtual int IsBlocking()
	{
		return 0;
	}

	virtual void OnNoCollision(DWORD dt);

	virtual void OnCollisionWith(LPCOLLISIONEVENT e);

public:

	Goomba(float x, float y);

	virtual void SetState(GoombaState state);

	virtual void OnEnable() override;

	virtual void OnExitCamera() override;

	void HandleSlope(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
};