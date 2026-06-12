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
	
	void GetBoundingBox(float& left, float& top, float& right, float& bottom) override;
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) override;
	void Render() override;

	int IsCollidable() override { return 1; };
	int IsBlocking() override { return 0; }
	void OnNoCollision(DWORD dt) override;

	void OnCollisionWith(LPCOLLISIONEVENT e) override;

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


