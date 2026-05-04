#include "PiranhaPlant.h"
#include "Mario.h"
#include "Fire.h"
#include "PlayScene.h"
void PiranhaPlant::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - PIRANHA_BBOX_WIDTH / 2;
	t = y - PIRANHA_BBOX_HEIGHT / 2 + 5.0f;
	r = l + PIRANHA_BBOX_WIDTH;
	b = t + PIRANHA_BBOX_HEIGHT;
}

void PiranhaPlant::OnNoCollision(DWORD dt)
{

};

void PiranhaPlant::OnCollisionWith(LPCOLLISIONEVENT e)
{
	if (!e->obj->IsBlocking()) return;

}

void PiranhaPlant::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	GameObject::Update(dt, coObjects);

	Mario* mario = (Mario*)((PlayScene*)SceneManager::GetInstance()
		->GetCurrentScene())->GetPlayer();

	float marioX, marioY;
	mario->GetPosition(marioX, marioY);


	float dx = abs(marioX - x);
	bool inRange = (dx <= 230.0f);

	if (inRange)
	{
		if (isWaiting)
		{
			if (GetTickCount64() - wait_start > PIRANHA_WAIT_TIME)
			{
				isWaiting = false;
				isGoingUp = !isGoingUp;
			}
			// Đang chờ, không làm gì cả
			return;
		}
		else {
			if (dx < 24.0f)
			{
				if (y == originalY) return;
			}
		}
		if (isGoingUp)
		{
			y -= PIRANHA_ARISE_SPEED * dt;
			if (y <= originalY - PIRANHA_HEIGHT)
			{
				y = originalY - PIRANHA_HEIGHT;

				if (!isAtTop)
				{
					isAtTop = true;
					top_start = GetTickCount64(); // bắt đầu đếm thời gian chờ
				}
				else
				{
					if (GetTickCount64() - top_start >= PIRANHA_TOP_WAIT)
					{
						isWaiting = true;
						wait_start = GetTickCount64();
						isAtTop = false;
					}
				}
			}
		}
		else
		{
			y += PIRANHA_ARISE_SPEED * dt;
			if (y >= originalY)
			{
				y = originalY;
				isWaiting = true;
				wait_start = GetTickCount64();
			}
		}
	}
	else
	{
		if (dx > 230.0f)
		{
			y += PIRANHA_ARISE_SPEED * dt;
			if (y >= originalY)
			{
				isGoingUp = false;
				y = originalY;
				isWaiting = true;
				wait_start = GetTickCount64();
			}
			isWaiting = true;
			wait_start = GetTickCount64();
			isAtTop = false; // reset để lần sau lên lại
		}
	}

	Collision::GetInstance()->Process(this, dt, coObjects);
}

void PiranhaPlant::Render()
{
	float marioX, marioY;

	Mario* mario = (Mario*)((PlayScene*)SceneManager::GetInstance()
		->GetCurrentScene())->GetPlayer();

	mario->GetPosition(marioX, marioY);
	int aniId = ID_ANI_PIRANHA;
	Animations::GetInstance()->Get(aniId)->Render(x, y);
	/*RenderBoundingBox();*/
}
