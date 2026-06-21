#include "HitEffect.h"

void HitEffect::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	y += vy * dt;

	if (GetTickCount64() - effect_start > 350)
	{
		this->Delete();
	}
}

void HitEffect::Render()
{
	Animations::GetInstance()->Get(ID_ANI_HIT_EFFECT)->Render(x, y);
}

void HitEffect::GetBoundingBox(float& left, float& top, float& right, float& bottom) { left = top = right = bottom = 0; };