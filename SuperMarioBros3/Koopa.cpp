#include "Koopa.h"
#include "Brick.h"
#include "Goomba.h"

Koopa::Koopa(float x, float y) :GameObject(x, y)
{
	this->ax = 0;
	this->ay = KOOPA_GRAVITY;
	die_start = -1;
	this->sensorfront = new SensorData(x + KOOPA_BBOX_WIDTH / 2, y, 2, KOOPA_BBOX_HEIGHT);
	this->sensorback = new SensorData(x - KOOPA_BBOX_WIDTH / 2, y, 2, KOOPA_BBOX_HEIGHT);
    // mặc định đi sang trái
	this->nx = -1;
	SetState(KoopaState::WALKING);
}

void Koopa::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (state == static_cast<int>(KoopaState::SHELL))
	{
		left = x - KOOPA_BBOX_WIDTH / 2;
		top = y - KOOPA_BBOX_HEIGHT_DIE / 2;
		right = left + KOOPA_BBOX_WIDTH;
		bottom = top + KOOPA_BBOX_HEIGHT_DIE;
	}
	else
	{
		left = x - KOOPA_BBOX_WIDTH / 2;
		top = y - KOOPA_BBOX_HEIGHT / 2;
		right = left + KOOPA_BBOX_WIDTH;
		bottom = top + KOOPA_BBOX_HEIGHT;
	}
}

void Koopa::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
};

void Koopa::OnCollisionWith(LPCOLLISIONEVENT e)
{
	Animations* animations = Animations::GetInstance();

	if (!e->obj->IsBlocking()) return;
	if (dynamic_cast<Koopa*>(e->obj)) return;

	if (e->ny != 0)
	{
		vy = 0;
	}
	else if (e->nx != 0)
	{
        vx = -vx;
		// cập nhật hướng hiển thị
		if (vx > 0) nx = 1;
		else nx = -1;
	}
	
	
}

void Koopa::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	// Giả sử hướng di chuyển là vx (dương là phải, âm là trái)
	float sensorOffset = (vx > 0) ? KOOPA_BBOX_WIDTH / 2 : -KOOPA_BBOX_WIDTH / 2;

	
	vy += ay * dt;
	vx += ax * dt;

	if ((this->state == static_cast<int>(KoopaState::SHELL)) && (GetTickCount64() - die_start > KOOPA_DIE_TIMEOUT))
	{
		SetState(KoopaState::WALKING);
		return;
	}

	GameObject::Update(dt, coObjects);
	Collision::GetInstance()->Process(this, dt, coObjects);
}


void Koopa::Render()
{
	int aniId = -1;
	float renderX = x;
	float renderY = y;
	if (this->state == static_cast<int>(KoopaState::SHELL))
	{
		aniId = ID_ANI_KOOPA_SHELL;
		renderY += 6;

	}
	else if (this->state == static_cast<int>(KoopaState::WALKING))
	{
		aniId = ID_ANI_KOOPA_WALKING;
	}

	if (aniId == -1) aniId = ID_ANI_KOOPA_WALKING;

	bool isFlip = (nx > 0);
	Animations::GetInstance()->Get(aniId)->Render(renderX, renderY, isFlip);
	//RenderBoundingBox();
}

void Koopa::SetState(KoopaState state)
{
	GameObject::SetState(static_cast<int>(state));
	switch (state)
	{
	case KoopaState::SHELL:
		die_start = GetTickCount64();
		ay = 0;
		vx = 0;
		vy = 0;
		break;
	case KoopaState::WALKING:
		this->ay = KOOPA_GRAVITY;
		vx = -KOOPA_WALKING_SPEED;
		nx = -1;
		break;
	}
}
