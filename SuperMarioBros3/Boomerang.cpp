#include "Boomerang.h"
#include "BoomerangBro.h"

Boomerang::Boomerang(float startX, float startY, int nx, BoomerangBro* owner) : GameObject(x, y)
{
	this->x = startX;
	this->y = startY;
	this->startX = startX;
	this->nx = nx;

	this->owner = owner;
	this->isReturning = false;
	this->returnTimer = 0;

	this->isDelayed = true;
	this->delayTimer = BOOMERANG_DELAY_TIME;

	this->vx = nx * BOOMERANG_SPEED_X;

	float t_total = BOOMERANG_MAX_DISTANCE / abs(this->vx);
	float t_peak = t_total / 2.0f;

	this->ay = (2.0f * BOOMERANG_MAX_HEIGHT) / (t_peak * t_peak);
	this->vy = -(this->ay * t_peak); // Dấu âm để bay lên trên
}

void Boomerang::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - 7.5f;
	t = y - 7.5f;
	r = l + 15.0f;
	b = t + 15.0f;
}

void Boomerang::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (isDelayed)
	{
		delayTimer -= (float)dt;

		if (owner != nullptr && owner->GetState() != 4) // 4 là BroState::DIE
		{
			this->x = owner->GetX();
			this->y = owner->GetY() - 2.0f;

			this->nx = owner->GetNx();
			this->startX = this->x;
			this->vx = this->nx * BOOMERANG_SPEED_X;
		}

		if (delayTimer <= 0)
		{
			isDelayed = false;
		}
		return;
	}

	if (!isReturning)
	{
		// PHASE 1: BAY RA XA (Quỹ đạo Parabol hoàn hảo)
		vy += ay * dt;
		x += vx * dt;
		y += vy * dt;

		if (abs(x - startX) >= BOOMERANG_MAX_DISTANCE)
		{
			isReturning = true;
			returnTimer = BOOMERANG_RETURN_TIME;
		}
	}
	else
	{
		// PHASE 2: BAY VỀ THEO ĐƯỜNG THẲNG (Linear Homing)
		if (owner != nullptr && owner->GetState() != 4)
		{
			returnTimer -= (float)dt;
			if (returnTimer <= 50.0f) returnTimer = 50.0f;

			float targetX = owner->GetX();
			float targetY = owner->GetY();

			float dx = targetX - x;
			float dy = targetY - y;

			vx = dx / returnTimer;
			vy = dy / returnTimer;

			x += vx * dt;
			y += vy * dt;

			float ol, ot, oright, ob;
			owner->GetBoundingBox(ol, ot, oright, ob);
			float bl, bt, br, bb;
			GetBoundingBox(bl, bt, br, bb);

			if (bl <= oright && br >= ol && bt <= ob && bb >= ot)
			{
				this->Delete();
				return;
			}
		}
		else
		{
			// PHASE 3: RƠI TỰ DO (Mất chủ)
			vy += ay * dt;
			x += vx * dt;
			y += vy * dt;
		}
	}

	CheckCameraStatus();
	if (!this->GetWasOnCamera())
	{
		this->Delete();
	}
}
void Boomerang::Render()
{
	if (isDelayed)
	{
		Animations::GetInstance()->Get(ID_ANI_BOOMERANG_IDLE)->Render(x, y);
	}
	else
	{
		Animations::GetInstance()->Get(ID_ANI_BOOMERANG)->Render(x, y);
	}
}