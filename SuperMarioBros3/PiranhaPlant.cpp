#include "PiranhaPlant.h"
#include "Mario.h"
#include "Fire.h"
#include "PlayScene.h"

void PiranhaPlant::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - PIRANHA_BBOX_WIDTH / 2;
	t = y - PIRANHA_BBOX_HEIGHT / 2;
	r = l + PIRANHA_BBOX_WIDTH;
	b = t + PIRANHA_BBOX_HEIGHT;
}

void PiranhaPlant::ShootFire(float marioX, float marioY)
{
	float mouthY = y - 5.0f;

	float dx = marioX - x;
	float dy = (marioY - 8.0f) - mouthY;

	float absDx = abs(dx);

	if (absDx < 0.0001f) absDx = 0.0001f;

	float angle = atan2(dy, absDx);
	float maxAngle = 30.0f * 3.14159f / 180.0f;

	if (angle > maxAngle)
	{
		angle = maxAngle;
	}
	else if (angle < -maxAngle)
	{
		angle = -maxAngle; 
	}

	int dirX = (dx > 0) ? 1 : -1;

	float fireVx = FIRE_SPEED * cos(angle) * dirX;
	float fireVy = FIRE_SPEED * sin(angle);

	Fire* fire = new Fire(x, mouthY);
	fire->SetSpeed(fireVx, fireVy);

	((PlayScene*)SceneManager::GetInstance()->GetCurrentScene())->AddObject(fire);
}

void PiranhaPlant::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	Mario* mario = (Mario*)((PlayScene*)SceneManager::GetInstance()->GetCurrentScene())->GetPlayer();

	if (mario == NULL) return;

	float marioX, marioY;
	mario->GetPosition(marioX, marioY);

	nx = (marioX >= x) ? 1 : -1;

	float dx = abs(marioX - x);
	bool inRange = (dx <= 230.0f);
	bool inSafeZone = (dx <= 24.0f); // Mario đứng sát miệng cống

	if (isWaiting)
	{
		if (y == originalY)
		{
			if (inSafeZone)
			{
				wait_start = GetTickCount64();
				return;
			}

			if (inRange && GetTickCount64() - wait_start > PIRANHA_WAIT_TIME)
			{
				isWaiting = false;
				isGoingUp = true;
			}
		}
		else // ĐANG TRÊN ĐỈNH ỐNG
		{
			ULONGLONG timeAtTop = GetTickCount64() - wait_start;
			if (plantType == PIRANHA_TYPE_RED_FIRE || plantType == PIRANHA_TYPE_GREEN_FIRE)
			{
				if (!hasShot && timeAtTop > (PIRANHA_TOP_WAIT / 2))
				{
					ShootFire(marioX, marioY);
					hasShot = true;
				}
			}

			if (timeAtTop > PIRANHA_TOP_WAIT)
			{
				isWaiting = false;
				isGoingUp = false;
			}
		}
		return;
	}

	if (isGoingUp)
	{
		y -= PIRANHA_ARISE_SPEED * dt;
		if (y <= originalY - ariseHeight)
		{
			y = originalY - ariseHeight;
			isWaiting = true;
			wait_start = GetTickCount64();
			hasShot = false;
		}
	}
	else // Đang chui xuống
	{
		y += PIRANHA_ARISE_SPEED * dt;
		if (y >= originalY)
		{
			// Chạm đáy
			y = originalY;
			isWaiting = true;
			wait_start = GetTickCount64();
		}
	}

	GameObject::Update(dt, coObjects);
	Collision::GetInstance()->Process(this, dt, coObjects);
}

void PiranhaPlant::Render()
{
	Mario* mario = (Mario*)((PlayScene*)SceneManager::GetInstance()->GetCurrentScene())->GetPlayer();
	if (mario == NULL) return;

	float marioX, marioY;
	mario->GetPosition(marioX, marioY);

	int aniId = -1;
	if (plantType == PIRANHA_TYPE_GREEN_NORMAL)
	{
		aniId = ID_ANI_PIRANHA_GREEN_NORMAL;
	}
	else if (plantType == PIRANHA_TYPE_RED_FIRE)
	{
		aniId = (marioY < y) ? ID_ANI_PIRANHA_RED_FIRE_UP : ID_ANI_PIRANHA_RED_FIRE_DOWN;
	}
	else if (plantType == PIRANHA_TYPE_GREEN_FIRE)
	{
		aniId = (marioY < y) ? ID_ANI_PIRANHA_GREEN_FIRE_UP : ID_ANI_PIRANHA_GREEN_FIRE_DOWN;
	}

	bool isFlip = (nx > 0);
	Animations::GetInstance()->Get(aniId)->Render(x, y, isFlip);
}