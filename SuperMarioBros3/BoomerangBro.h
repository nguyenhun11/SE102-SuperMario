#pragma once
#include "RespawnableEnemy.h"

#define BOOMERANG_BRO_GRAVITY           0.002f
#define BOOMERANG_BRO_WALKING_SPEED     0.03f
#define BOOMERANG_BRO_DEFLECT_SPEED     0.35f

#define BOOMERANG_BRO_BBOX_WIDTH        16.0f
#define BOOMERANG_BRO_BBOX_HEIGHT       22.0f
#define PATROL_RANGE                    16.0f // Biên độ đi lại (Tổng quãng đường 48px)

#define ID_ANI_BOOMERANG_BRO_WALKING    20400
#define ID_ANI_BOOMERANG_BRO_THROWING   20401

enum class BroState
{
	ALIVE = 0,
	DIE = 1
};

class BoomerangBro : public RespawnableEnemy
{
protected:
	float startX;
	bool isFlippedVertical;

	// BỘ QUẢN LÝ NÉM ĐỘC LẬP
	int throwCount;              // Đếm số boomerang đã ném trong chu kỳ
	ULONGLONG throwStart;        // Thời điểm bắt đầu chờ
	DWORD throwCooldown;         // Thời gian chờ đến phát ném tiếp theo

	bool isThrowing;             // Cờ hiệu đổi Animation
	ULONGLONG throwAniStart;

	bool isIdleAtEdge;
	ULONGLONG idleStart;

	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

	virtual int IsCollidable();
	virtual int IsBlocking() { return 0; }
	virtual void OnNoCollision(DWORD dt);
	virtual void OnCollisionWith(LPCOLLISIONEVENT e);

	virtual void OnEnable() override;
	virtual void OnExitCamera() override;

	void ThrowBoomerang();

public:
	BoomerangBro(float x, float y);
	virtual void SetState(int state);
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
};