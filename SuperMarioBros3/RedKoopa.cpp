#include "RedKoopa.h"
#include "Brick.h"
#include "Goomba.h"

RedKoopa::RedKoopa(float x, float y) : Koopa(x, y)
{
	
}

void RedKoopa::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	Koopa::GetBoundingBox(left, top, right, bottom);
}

void RedKoopa::OnNoCollision(DWORD dt)
{
	Koopa::OnNoCollision(dt);
}

void RedKoopa::OnCollisionWith(LPCOLLISIONEVENT e)
{
	Koopa::OnCollisionWith(e);
}

void RedKoopa::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	Koopa::Update(dt, coObjects);
}

void RedKoopa::Render()
{
	int aniId = -1;
	float renderX = x;
	float renderY = y; // Tọa độ y vật lý chuẩn chỉnh, không cần cộng bù trừ thủ công nữa

	if (this->state == static_cast<int>(KoopaState::SHELL) || this->state == static_cast<int>(KoopaState::SHELL_UPWARD))
	{
		aniId = ID_ANI_RED_KOOPA_SHELL;
	}
	else if (this->state == static_cast<int>(KoopaState::WALKING))
	{
		aniId = ID_ANI_RED_KOOPA_WALKING;
	}
	else if (this->state == static_cast<int>(KoopaState::SHELL_MOVING))
	{
		aniId = ID_ANI_RED_KOOPA_SHELL_MOVING;
	}
	else if (this->state == static_cast<int>(KoopaState::SHAKING))
	{
		aniId = ID_ANI_RED_KOOPA_SHELL_SHAKING;

		// Hiệu ứng rung lắc nhẹ bằng toán tử Modulo thời gian
		if ((GetTickCount64() / 50) % 2 == 0)
		{
			renderX += 1.0f;
		}
		else
		{
			renderX -= 1.0f;
		}
	}

	if (aniId == -1) aniId = ID_ANI_RED_KOOPA_WALKING;

	// Lật ảnh sprite (isFlip = true nếu đi sang phải)
	bool isFlip = (nx > 0);
	Animations::GetInstance()->Get(aniId)->Render(renderX, renderY, isFlip, isFlippedVertical);

	// Chỉ Render Sensor để theo dõi trực quan khi đang đi bộ
	if (state == static_cast<int>(KoopaState::WALKING) && sensorfront != nullptr)
	{
		sensorfront->Render();
	}
}

void RedKoopa::SetState(KoopaState state)
{
	Koopa::SetState(state);
}