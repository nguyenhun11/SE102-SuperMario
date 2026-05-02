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
	animations->Get(ID_ANI_BRICK_ACTIVE)->Render(x, y);
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

		PlayScene* scene = (PlayScene*)SceneManager::GetInstance()->GetCurrentScene();

		// Mảnh trên - trái
		BrickDebris* topLeft = new BrickDebris(x, y, -DEBRIS_BOUNCE_SPEED_X, -DEBRIS_BOUNCE_SPEED_Y_HIGH);
		scene->AddObject(topLeft);

		// Mảnh trên - phải
		BrickDebris* topRight = new BrickDebris(x, y, DEBRIS_BOUNCE_SPEED_X, -DEBRIS_BOUNCE_SPEED_Y_HIGH);
		scene->AddObject(topRight);

		// Mảnh dưới - trái
		BrickDebris* bottomLeft = new BrickDebris(x, y, -DEBRIS_BOUNCE_SPEED_X, -DEBRIS_BOUNCE_SPEED_Y_LOW);
		scene->AddObject(bottomLeft);

		// Mảnh dưới - phải
		BrickDebris* bottomRight = new BrickDebris(x, y, DEBRIS_BOUNCE_SPEED_X, -DEBRIS_BOUNCE_SPEED_Y_LOW);
		scene->AddObject(bottomRight);
	}
}