#include "MapMario.h"
#include "Animations.h"
#include "debug.h"

MapMario::MapMario(float x, float y) : GameObject(x, y)
{
	isMoving = false;
	currentNode = NULL;

	renderFlip = false;
	flipTimer = 0;

	SetMarioForm(MarioMapForm::SMALL);
	SetState(MARIO_MAP_STATE_IDLE);
}

void MapMario::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	ULONGLONG now = GetTickCount64();
	if (now - flipTimer > 150)
	{
		renderFlip = !renderFlip;
		flipTimer = now;
	}

	if (coObjects == NULL) return;

	if (currentNode == NULL)
	{
		for (size_t i = 0; i < coObjects->size(); i++)
		{
			MapNode* node = dynamic_cast<MapNode*>(coObjects->at(i));
			if (node != NULL)
			{
				// Quét xem vị trí ban đầu của Mario có đang đè lên Node nào không
				if (abs(this->x - node->GetX()) <= 1.5f && abs(this->y - node->GetY()) <= 1.5f)
				{
					this->currentNode = node; // Lưu lại trạm xuất phát!
					break;
				}
			}
		}
	}

	if (!isMoving) return;

	x += vx * dt;
	y += vy * dt;

	for (size_t i = 0; i < coObjects->size(); i++)
	{
		LPGAMEOBJECT obj = coObjects->at(i);
		MapNode* node = dynamic_cast<MapNode*>(obj);

		if (node != NULL && node != currentNode)
		{
			if (abs(this->x - node->GetX()) <= 1.5f && abs(this->y - node->GetY()) <= 1.5f)
			{
				this->x = node->GetX();
				this->y = node->GetY();
				this->vx = 0;
				this->vy = 0;

				this->isMoving = false;
				this->currentNode = node;

				this->stopTimer = GetTickCount64();

				SetState(MARIO_MAP_STATE_IDLE);
				return;
			}
		}
	}
}

void MapMario::Render()
{
	int spriteId = ID_SPRITE_MAPMARIO_SMALL + (int)form;

	bool isFlip = renderFlip;

	float draw_x = x;
	float draw_y = y;

	switch (form)
	{
	case MarioMapForm::SUPER:
		draw_y -= 8.0f;
		break;

	case MarioMapForm::P:
		draw_y -= 8.0f;

		if (renderFlip == true)
		{
			spriteId *= -1; // Lấy tấm ảnh thứ 2 của form P
		}

		isFlip = false;
		break;

	case MarioMapForm::RACOON:
		draw_y -= 8.0f;
		break;

	case MarioMapForm::SMALL:
	default:
		break;
	}
	Sprites::GetInstance()->Get(spriteId)->DrawOnCamera(draw_x, draw_y, isFlip);
}

void MapMario::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + 16;
	b = y + 16;
}

void MapMario::SetState(int state)
{
	switch (state)
	{
	case MARIO_MAP_STATE_IDLE:
		vx = 0;
		vy = 0;
		isMoving = false;
		break;
	case MARIO_MAP_STATE_WALKING_LEFT:
		vx = -MARIO_MAP_SPEED;
		vy = 0;
		isMoving = true;
		break;
	case MARIO_MAP_STATE_WALKING_RIGHT:
		vx = MARIO_MAP_SPEED;
		vy = 0;
		isMoving = true;
		break;
	case MARIO_MAP_STATE_WALKING_UP:
		vx = 0;
		vy = -MARIO_MAP_SPEED;
		isMoving = true;
		break;
	case MARIO_MAP_STATE_WALKING_DOWN:
		vx = 0;
		vy = MARIO_MAP_SPEED;
		isMoving = true;
		break;
	}
	GameObject::SetState(state);
}

void MapMario::MoveLeft()
{
	// Nếu vừa tới trạm chưa đủ 150ms -> Bỏ qua lệnh
	if (GetTickCount64() - stopTimer < 150) return;

	if (!isMoving && (currentNode == NULL || currentNode->canLeft))
		SetState(MARIO_MAP_STATE_WALKING_LEFT);
}

void MapMario::MoveRight()
{
	if (GetTickCount64() - stopTimer < 150) return;

	if (!isMoving && (currentNode == NULL || currentNode->canRight))
		SetState(MARIO_MAP_STATE_WALKING_RIGHT);
}

void MapMario::MoveUp()
{
	if (GetTickCount64() - stopTimer < 150) return;

	if (!isMoving && (currentNode == NULL || currentNode->canUp))
		SetState(MARIO_MAP_STATE_WALKING_UP);
}

void MapMario::MoveDown()
{
	if (GetTickCount64() - stopTimer < 150) return;

	if (!isMoving && (currentNode == NULL || currentNode->canDown))
		SetState(MARIO_MAP_STATE_WALKING_DOWN);
}