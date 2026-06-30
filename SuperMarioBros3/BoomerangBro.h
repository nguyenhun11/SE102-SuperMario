#pragma once
#include "RespawnableEnemy.h"

#define BOOMERANG_BRO_GRAVITY           0.002f
#define BOOMERANG_BRO_WALKING_SPEED     0.03f
#define BOOMERANG_BRO_JUMP_SPEED        0.25f
#define BOOMERANG_BRO_DEFLECT_SPEED     0.35f

#define BOOMERANG_BRO_BBOX_WIDTH        16.0f
#define BOOMERANG_BRO_BBOX_HEIGHT       22.0f
#define PATROL_DISTANCE                 16.0f 

#define ID_ANI_BOOMERANG_BRO_WALKING    20400
#define ID_ANI_BOOMERANG_BRO_THROWING   20401

enum class BroState
{
	WALKING = 0,
	WIND_UP_1 = 1,
	WAIT_THROW_2 = 2,
	WIND_UP_2 = 3,
	DIE = 4
};

class BoomerangBro : public RespawnableEnemy
{
protected:
	float startX;
	bool isFlippedVertical;
	bool isOnGround;

	ULONGLONG action_start;
	ULONGLONG jump_start;

	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

	virtual int IsCollidable();
	virtual int IsBlocking() { return 0; }

	virtual void OnNoCollision(DWORD dt);
	virtual void OnCollisionWith(LPCOLLISIONEVENT e);

	virtual void OnEnable() override; // Hàm mấu chốt để hồi sinh
	virtual void OnExitCamera() override;

	void ThrowBoomerang();

public:
	BoomerangBro(float x, float y);
	virtual void SetState(int state);
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom); // PUBLIC
};