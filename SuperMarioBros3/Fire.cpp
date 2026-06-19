#include "Fire.h"
#include "Mario.h"
#include "Camera.h"

void Fire::Render()
{
	Animations::GetInstance()->Get(ID_ANI_FIRE)->Render(x, y);
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

	// Bay ra ngoài Camera → tự xóa
	float camX, camY;
	Camera::GetInstance()->GetCamPos(camX, camY);

	if (x < camX - FIRE_BBOX_WIDTH ||
		x > camX + SCREEN_WIDTH + FIRE_BBOX_WIDTH ||
		y < camY - FIRE_BBOX_HEIGHT ||
		y > camY + SCREEN_HEIGHT + FIRE_BBOX_HEIGHT)
	{
		isDeleted = true;
		return;
	}

	Collision::GetInstance()->Process(this, dt, coObjects);
}
