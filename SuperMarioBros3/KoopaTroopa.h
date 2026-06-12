#pragma once
#include "Koopa.h"

#define KOOPA_PARATROOPA_BASE_FLYING_SPEED 0.35f
#define KOOPATROOPA_BBOX_WIDTH 20.f
#define KOOPATROOPA_BBOX_HEIGHT 31.f



class KoopaTroopa : public Koopa
{
private:
	
	void GetBoundingBox(float& left, float& top, float& right, float& bottom) override;
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) override;
	void Render() override;


	void OnCollisionWith(LPCOLLISIONEVENT e) override;

public:
	KoopaTroopa(float x, float y);
	void SetState(KoopaState state) override;

};

