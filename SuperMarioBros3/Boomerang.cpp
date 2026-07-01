#include "Boomerang.h"
#include "BoomerangBro.h"

Boomerang::Boomerang(float x, float y, int nx, BoomerangBro* owner) : GameObject(x, y)
{
	this->nx = nx;
	this->owner = owner;
	this->startX = x;
	this->isReturning = false;
	this->returnTimer = 0;

	this->vx = nx * BOOMERANG_SPEED_X;
	this->vy = -0.05f;
	this->ay = BOOMERANG_GRAVITY;
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
	if (!isReturning)
	{
		// PHASE 1: BAY RA XA (Quỹ đạo ngang Parabol)
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
		if (owner != nullptr && owner->GetState() != 4) // 4 là BroState::DIE
		{
			returnTimer -= (float)dt;
			if (returnTimer <= 0) returnTimer = 1.0f;

			float targetX = owner->GetX();
			float targetY = owner->GetY();

			// Tính quãng đường thẳng
			float dx = targetX - x;
			float dy = targetY - y;

			// THUẬT TOÁN ĐƯỜNG THẲNG: vx = S/t (Hoàn toàn triệt tiêu gia tốc ay)
			vx = dx / returnTimer;
			vy = dy / returnTimer;

			// Cập nhật tọa độ
			x += vx * dt;
			y += vy * dt;

			// Logic bắt vũ khí bằng AABB
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
			// PHASE 3: MẤT CHỦ -> Rơi tự do (Bật lại gia tốc)
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
	Animations::GetInstance()->Get(ID_ANI_BOOMERANG)->Render(x, y);
}