#include "MapMario.h"
#include "GameManager.h"
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
	// 1. Lập ảnh animation
	ULONGLONG now = GetTickCount64();
	if (now - flipTimer > 150)
	{
		renderFlip = !renderFlip;
		flipTimer = now;
	}

	// 2. Tìm node ban đầu
	if (coObjects == NULL) return;
	if (currentNode == NULL)
	{
		for (size_t i = 0; i < coObjects->size(); i++)
		{
			MapNode* node = dynamic_cast<MapNode*>(coObjects->at(i));
			if (node != NULL)
			{
				if (abs(this->x - node->GetX()) <= 1.5f && abs(this->y - node->GetY()) <= 1.5f)
				{
					this->currentNode = node;

					// Nếu là lần đầu tiên mở game, CẢ HAI đều là Start Node
					if (GameManager::GetInstance()->mapMarioPrevX == -1.0f)
					{
						GameManager::GetInstance()->mapMarioPrevX = this->x;
						GameManager::GetInstance()->mapMarioPrevY = this->y;
						GameManager::GetInstance()->mapMarioCurrentX = this->x;
						GameManager::GetInstance()->mapMarioCurrentY = this->y;
					}
					break;
				}
			}
		}
	}

	// 3. Bị đẩy lùi nếu thua game
	// 3. Bị đẩy lùi nếu thua game
	if (GameManager::GetInstance()->isReturningFromFail)
	{
		float px = GameManager::GetInstance()->mapMarioPrevX;
		float py = GameManager::GetInstance()->mapMarioPrevY;

		float dx = px - this->x;
		float dy = py - this->y;
		float distance = sqrt(dx * dx + dy * dy);

		// Tính quãng đường tối đa Mario sẽ di chuyển trong frame này
		float step = MARIO_MAP_SPEED * dt;

		DebugOut(L"[MAP DEBUG] Fail Flag: %d | Current(%.1f, %.1f) | SafePoint(%.1f, %.1f) | Distance: %.1f \n",
			GameManager::GetInstance()->isReturningFromFail, this->x, this->y, px, py, distance);

		// NẾU KHOẢNG CÁCH CÒN LỚN HƠN BƯỚC NHẢY -> TIẾP TỤC TRƯỢT
		if (distance > step)
		{
			this->vx = (dx / distance) * MARIO_MAP_SPEED;
			this->vy = (dy / distance) * MARIO_MAP_SPEED;
			this->isMoving = true;

			this->x += vx * dt;
			this->y += vy * dt;

			return; // QUAN TRỌNG: Return ngay tại đây! 
		}
		// ĐÃ TỚI ĐÍCH (Safe Point) - Bắt dính Mario vào tọa độ px, py
		else
		{
			this->x = px;
			this->y = py;
			this->vx = 0;
			this->vy = 0;
			this->isMoving = false;

			GameManager::GetInstance()->isReturningFromFail = false;
			GameManager::GetInstance()->mapMarioCurrentX = this->x;
			GameManager::GetInstance()->mapMarioCurrentY = this->y;

			this->stopTimer = GetTickCount64();
			SetState(MARIO_MAP_STATE_IDLE);

			this->currentNode = NULL;
			return;
		}
	}

	// 4. Di chuyển
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

				GameManager::GetInstance()->mapMarioCurrentX = this->x;
				GameManager::GetInstance()->mapMarioCurrentY = this->y;

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

	if (!isMoving && (currentNode == NULL || currentNode->canLeft)) {
		SetState(MARIO_MAP_STATE_WALKING_LEFT);
	}
}

void MapMario::MoveRight()
{
	if (GetTickCount64() - stopTimer < 150) return;

	if (!isMoving && (currentNode == NULL || currentNode->canRight)) {
		SetState(MARIO_MAP_STATE_WALKING_RIGHT);
	}
}

void MapMario::MoveUp()
{
	if (GetTickCount64() - stopTimer < 150) return;

	if (!isMoving && (currentNode == NULL || currentNode->canUp)) {
		SetState(MARIO_MAP_STATE_WALKING_UP);
	}
}

void MapMario::MoveDown()
{
	if (GetTickCount64() - stopTimer < 150) return;

	if (!isMoving && (currentNode == NULL || currentNode->canDown)) {
		SetState(MARIO_MAP_STATE_WALKING_DOWN);
	}
}

void MapMario::EnterNode()
{
	if (isMoving || currentNode == NULL) return;

	StageNode* stageNode = dynamic_cast<StageNode*>(currentNode);

	if (stageNode != NULL)
	{
		int sceneId = (int)stageNode->GetSceneId();

		if (sceneId > 0)
		{
			GameManager::GetInstance()->mapMarioCurrentX = this->x;
			GameManager::GetInstance()->mapMarioCurrentY = this->y;

			SceneManager::GetInstance()->InitiateSwitchScene(sceneId);
		}
	}
}