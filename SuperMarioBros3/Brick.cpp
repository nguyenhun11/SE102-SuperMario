#include "Brick.h"
#include "debug.h"

Brick::Brick(float x, float y, int itemType) : GameObject(x, y)
{
	this->startY = y;
	this->currentState = BrickState::ACTIVE;
	this->state = static_cast<int>(BrickState::ACTIVE);
	this->vy = 0;
	this->zIndex = 5;

	// Ép kiểu int từ file text truyền vào thành Enum
	this->containedItem = static_cast<BrickItem>(itemType);
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
			if (containedItem != BrickItem::NONE)
			{
				SetState(BrickState::EMPTY);
				SpawnItem(); // Nhả đồ ra
			}
			else
			{
				SetState(BrickState::ACTIVE); // Gạch rỗng thì khôi phục lại bình thường
			}
		}
	}
}

void Brick::Render()
{
	if (currentState == BrickState::BROKEN) return;

	int aniId = ID_ANI_BRICK_ACTIVE;

	if (currentState == BrickState::EMPTY || (currentState == BrickState::BOUNCING && containedItem != BrickItem::NONE))
	{
		aniId = ID_ANI_BRICK_EMPTY;
	}

	Animations* animations = Animations::GetInstance();
	animations->Get(aniId)->Render(x, y);
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
	case BrickState::EMPTY:
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

void Brick::SpawnItem()
{
	PlayScene* playScene = (PlayScene*)SceneManager::GetInstance()->GetCurrentScene();

	if (containedItem == BrickItem::MUSHROOM)
	{
		Mushroom* mushroom = new Mushroom(x, y);
		playScene->AddObject(mushroom);
	}
	else if (containedItem == BrickItem::P_SWITCH)
	{
		//  P-Switch sau
	}
	else if (containedItem == BrickItem::ONE_UP_MUSHROOM)
	{
		OneUpMushroom* mr = new OneUpMushroom(x, y);
		playScene->AddObject(mr);
	}
}