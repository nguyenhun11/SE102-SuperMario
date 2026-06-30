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
		// PHASE 1: BAY RA XA 
		vy += ay * dt;
		x += vx * dt;
		y += vy * dt;

		// Vượt quá tầm xa thì bắt đầu chu kỳ quay về
		if (abs(x - startX) >= BOOMERANG_MAX_DISTANCE)
		{
			isReturning = true;
			returnTimer = BOOMERANG_RETURN_TIME;
		}
	}
	else
	{
		// PHASE 2: BAY VỀ VỚI QUỸ ĐẠO CONG (Kinematic Parabolic Homing)
		if (owner != nullptr && owner->GetState() != 4) // 4 là BroState::DIE
		{
			returnTimer -= (float)dt; // Trừ dần thời gian
			if (returnTimer <= 0) returnTimer = 1.0f; // Chống lỗi chia cho 0 khi sát nút

			// Tọa độ mục tiêu (bàn tay Bro)
			float targetX = owner->GetX();
			float targetY = owner->GetY() - 4.0f;

			// Quãng đường cần bay (S)
			float dx = targetX - x;
			float dy = targetY - y;

			// ÁP DỤNG CÔNG THỨC: v0 = S/t - 0.5*a*t
			vx = dx / returnTimer;
			vy = (dy / returnTimer) + 0.5f * BOOMERANG_GRAVITY * returnTimer;

			// Áp dụng tọa độ (Không cần vy += ay * dt nữa vì vy đã tính toán sẵn gia tốc ở trên)
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
			// PHASE 3: MẤT CHỦ (Chủ chết) -> Trở về trạng thái rơi tự do
			vy += ay * dt;
			x += vx * dt; // vx giữ nguyên dư âm của frame cuối cùng
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