#include "Mushroom.h"
#include "Collision.h"
#include "PlayScene.h"
#include "Mario.h"
#include "SoundManager.h"
#include "Slope.h"

Mushroom::Mushroom(float x, float y) : GameObject(x, y)
{
	this->startY = y;
	SetState(MushroomState::SPAWNING);
	SoundManager::GetInstance()->Play("mushroom_appear");
	zIndex = 4;
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

	HandleSlope(dt, coObjects);
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

void Mushroom::HandleSlope(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (state == static_cast<int>(MushroomState::SPAWNING)) return;

	float l, t, r, b;
	GetBoundingBox(l, t, r, b);
	float mushroomBottomY = b;     
	float bboxHeight = b - t;

	bool foundSlope = false;

	for (size_t i = 0; i < coObjects->size(); i++)
	{
		LPGAMEOBJECT obj = coObjects->at(i);
		if (dynamic_cast<Slope*>(obj)) // Kiểm tra nếu object là dốc
		{
			Slope* slope = dynamic_cast<Slope*>(obj);
			float sl, st, sr, sb;
			slope->GetBoundingBox(sl, st, sr, sb);

			float mushroomCenterX = x; 

			if (mushroomCenterX >= sl && mushroomCenterX <= sr && mushroomBottomY >= st && mushroomBottomY <= sb)
			{
				float expectedY = slope->GetSurfaceY(mushroomCenterX);
				float epsilon = max(4.0f, vy * dt);
				if (mushroomBottomY >= expectedY - epsilon && vy >= 0)
				{
					y = expectedY - bboxHeight / 2;
					vy = 0;                        
					foundSlope = true;
				}
			}
		}
	}

	isOnSlope = foundSlope;
}