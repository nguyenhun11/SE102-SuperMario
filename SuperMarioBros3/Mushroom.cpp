#include "Mushroom.h"
#include "Collision.h"
#include "PlayScene.h"
#include "Mario.h"

Mushroom::Mushroom(float x, float y) : GameObject(x, y)
{
	this->startY = y;
	SetState(MushroomState::SPAWNING);
	zIndex = 1;
}

void Mushroom::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - MUSHROOM_BBOX_WIDTH / 2;
	t = y - MUSHROOM_BBOX_HEIGHT / 2;
	r = l + MUSHROOM_BBOX_WIDTH;
	b = t + MUSHROOM_BBOX_HEIGHT;
}

void Mushroom::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
}

void Mushroom::OnCollisionWith(LPCOLLISIONEVENT e)
{
	if (dynamic_cast<Mario*>(e->obj)) return;

	if (!e->obj->IsBlocking()) return;

	if (e->ny != 0)
	{
		vy = 0;
	}
	else if (e->nx != 0)
	{
		vx = -vx; // Đụng tường thật thì mới quay đầu
	}
}

void Mushroom::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	// trạng thái spawning thì trồi lên
	if (state == static_cast<int>(MushroomState::SPAWNING))
	{
		y += vy * dt;

		if (startY - y >= 16.0f)
		{
			y = startY - 16.0f; 
			SetState(MushroomState::WALKING); // chuyển sang trạng thái chạy qua chạy lại
		}
		return;
	}

	vy += MUSHROOM_GRAVITY * dt;
	Collision::GetInstance()->Process(this, dt, coObjects);
}

void Mushroom::Render()
{
	LPANIMATION ani = Animations::GetInstance()->Get(ID_ANIM_MUSHROOM);
	if (ani == NULL)
	{
		return;
	}
	ani->Render(x, y);

	//RenderBoundingBox(); // debug bounding shape
}

void Mushroom::SetState(MushroomState newState)
{
	switch (newState)
	{
	case MushroomState::SPAWNING:
		vy = -MUSHROOM_SPAWN_SPEED;
		vx = 0;
		break;
	case MushroomState::WALKING:
		PlayScene* playScene = (PlayScene*)SceneManager::GetInstance()->GetCurrentScene();
		Mario* mario = (Mario*)playScene->GetPlayer();
		vy = 0;
		if (x >= mario->GetX())
			vx = MUSHROOM_WALKING_SPEED;
		else
			vx = -MUSHROOM_WALKING_SPEED;
		break;
	}

	GameObject::SetState(static_cast<int>(newState));
}