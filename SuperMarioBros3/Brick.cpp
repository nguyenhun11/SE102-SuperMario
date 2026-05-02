#include "Brick.h"
#include "debug.h"

Brick::Brick(float x, float y) : GameObject(x, y)
{
	this->startY = y;
	this->currentState = BrickState::ACTIVE;
	this->state = static_cast<int>(BrickState::ACTIVE);
	this->vy = 0;
	this->zIndex = 5;
}

void Brick::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (currentState == BrickState::BROKEN) return;

	y += vy * dt;

	if (currentState == BrickState::BOUNCING)
	{
		if (startY - y >= BRICK_BOUNCE_HEIGHT && vy < 0)
		{
			vy = BRICK_BOUNCE_SPEED;
		}
		else if (y >= startY && vy > 0)
		{
			y = startY;
			vy = 0;
			SetState(BrickState::ACTIVE); // Gạch nảy xong thì khôi phục bình thường
		}
	}
}

void Brick::Render()
{
	if (currentState == BrickState::BROKEN) return;

	Animations* animations = Animations::GetInstance();
	animations->Get(ID_ANI_BRICK)->Render(x, y);
}

void Brick::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	if (currentState == BrickState::BROKEN) return;

	l = x - BRICK_BBOX_WIDTH / 2;
	t = y - BRICK_BBOX_HEIGHT / 2;
	r = l + BRICK_BBOX_WIDTH;
	b = t + BRICK_BBOX_HEIGHT;
}

void Brick::SetState(BrickState state)
{
	this->currentState = state;

	switch (state)
	{
	case BrickState::BOUNCING:
		vy = -BRICK_BOUNCE_SPEED;
		break;
	case BrickState::ACTIVE:
		vy = 0;
		break;
	case BrickState::BROKEN:
		// Đánh dấu xóa đối tượng khỏi danh sách Update/Render của Scene
		this->Delete();
		break;
	}

	GameObject::SetState(static_cast<int>(state));
}

void Brick::Break()
{
	if (currentState != BrickState::BROKEN)
	{
		SetState(BrickState::BROKEN);

		// TODO: Khởi tạo 4 mảnh vỡ (Debris) và AddObject vào PlayScene tại đây
		// DebugOut(L"[INFO] Brick broken at %f, %f\n", x, y);
	}
}