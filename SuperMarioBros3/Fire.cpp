#include "Fire.h"

void Fire::Render()
{
	int aniId = ID_ANI_FIRE;
	Animations::GetInstance()->Get(aniId)->Render(x, y);
	//RenderBoundingBox();
}

void Fire::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - FIRE_BBOX_WIDTH / 2;
	t = y - FIRE_BBOX_HEIGHT / 2;
	r = l + FIRE_BBOX_WIDTH;
	b = t + FIRE_BBOX_HEIGHT;
}

void Fire::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	GameObject::Update(dt, coObjects);
	x += vx * dt;
	y += vy * dt;
	// Kiểm tra thời gian sống
	if (GetTickCount64() - fire_start >= FIRE_LIFETIME)
	{
		this->isDeleted = true;
		return;
	}

	Collision::GetInstance()->Process(this, dt, coObjects);
}