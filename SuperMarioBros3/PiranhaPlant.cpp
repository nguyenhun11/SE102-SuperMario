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

void PiranhaPlant::OnNoCollision(DWORD dt) {}

void PiranhaPlant::OnCollisionWith(LPCOLLISIONEVENT e)
{
	// Đụng Mario → Mario mất máu
	Mario* mario = dynamic_cast<Mario*>(e->obj);
	if (mario)
	{
		mario->TakeDamage();
	}
}

void PiranhaPlant::ShootFire(float marioX, float marioY)
{
	float dx = marioX - x;
	float dy = marioY - y;
	float dist = sqrt(dx * dx + dy * dy);
	if (dist == 0) return;

	float fireVx = FIRE_SPEED * dx / dist;
	float fireVy = FIRE_SPEED * dy / dist;

	Fire* fire = new Fire(x, y);
	fire->SetSpeed(fireVx, fireVy);

	((PlayScene*)SceneManager::GetInstance()->GetCurrentScene())->AddObject(fire);
}

void PiranhaPlant::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
		GameObject::Update(dt, coObjects);

	Mario* mario = (Mario*)((PlayScene*)SceneManager::GetInstance()
		->GetCurrentScene())->GetPlayer();

	float marioX, marioY;
	mario->GetPosition(marioX, marioY);

	// Lật mặt về phía Mario
	nx = (marioX >= x) ? 1 : -1;

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
					top_start = GetTickCount64();
					fire_timer = GetTickCount64(); // bắt đầu timer bắn lửa
				}
				else
				{
					// Bắn lửa theo interval khi đang ở trên đỉnh
					if (GetTickCount64() - fire_timer >= PIRANHA_FIRE_INTERVAL)
					{
						ShootFire(marioX, marioY);
						fire_timer = GetTickCount64();
					}

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
		// Mario ra ngoài tầm → rút xuống
		y += PIRANHA_ARISE_SPEED * dt;
		if (y >= originalY)
		{
			isGoingUp = false;
			y = originalY;
			isWaiting = true;
			wait_start = GetTickCount64();
			isAtTop = false;
		}
	}

	Collision::GetInstance()->Process(this, dt, coObjects);
}

void PiranhaPlant::Render()
{
	Mario* mario = (Mario*)((PlayScene*)SceneManager::GetInstance()
		->GetCurrentScene())->GetPlayer();

	float marioX, marioY;
	mario->GetPosition(marioX, marioY);

	// Chọn animation: nhìn lên nếu Mario ở cao hơn hoa, nhìn xuống nếu ngược lại
	int aniId;
	if (marioY < y)
		aniId = ID_ANI_PIRANHA_LOOK_UP;
	else
		aniId = ID_ANI_PIRANHA_LOOK_DOWN;

	// Lật ảnh theo hướng Mario (nx = -1 thì flip)
	bool isFlip = (nx == -1);
	Animations::GetInstance()->Get(aniId)->Render(x, y, isFlip);

	/*RenderBoundingBox();*/
}
