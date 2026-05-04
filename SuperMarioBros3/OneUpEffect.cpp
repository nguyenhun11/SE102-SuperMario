#include "OneUpEffect.h"

OneUpEffect::OneUpEffect(float x, float y) : GameObject(x, y)
{
	effect_start = GetTickCount64();
	vy = -0.07f;
	vx = 0;
}
void OneUpEffect::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	y += vy * dt;

	if (GetTickCount64() - effect_start > 1000)
	{
		this->Delete();
	}
}

void OneUpEffect::Render()
{
	Animations::GetInstance()->Get(ID_ANI_ONE_UP_EFFECT)->Render(x, y);
}

void OneUpEffect::GetBoundingBox(float& left, float& top, float& right, float& bottom) { left = top = right = bottom = 0; };