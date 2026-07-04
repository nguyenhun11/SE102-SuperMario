#include "MapMario.h"
#include "GameManager.h"
#include "Animations.h"
#include "debug.h"
#include "SoundManager.h"

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
	if (GameManager::GetInstance()->isReturningFromFail)
	{
		float px = GameManager::GetInstance()->mapMarioPrevX;
		float py = GameManager::GetInstance()->mapMarioPrevY;

		float dx = px - this->x;
		float dy = py - this->y;
		float distance = sqrt(dx * dx + dy * dy);

		float step = MARIO_MAP_SPEED * dt;

		// NẾU KHOẢNG CÁCH CÒN LỚN HƠN BƯỚC NHẢY -> TIẾP TỤC TRƯỢT
		if (distance > step)
		{
			this->vx = (dx / distance) * MARIO_MAP_SPEED;
			this->vy = (dy / distance) * MARIO_MAP_SPEED;
			this->isMoving = true;

			this->x += vx * dt;
			this->y += vy * dt;

			static ULONGLONG skidTimer = 0;
			if (GetTickCount64() - skidTimer > 100)
			{
				SoundManager::GetInstance()->Play("twirl"); // Tên file âm thanh trượt của ông
				skidTimer = GetTickCount64();
			}

			return;
		}
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

			SoundManager::GetInstance()->PlayBGM("bgm_world");
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

bool MapMario::CanMoveToDirection(int nx, int ny)
{
	// 1. KIỂM TRA ĐƯỜNG CƠ BẢN (Theo file txt: l, r, u, d)
	if (currentNode == NULL) return true;

	bool hasPath = false;
	if (nx == -1 && currentNode->canLeft) hasPath = true;
	if (nx == 1 && currentNode->canRight) hasPath = true;
	if (ny == -1 && currentNode->canUp) hasPath = true;
	if (ny == 1 && currentNode->canDown) hasPath = true;

	if (!hasPath) return false;

	// 2. DEV MODE: Mở khóa vạn năng, đi đâu cũng được không cần phá đảo
	if (GameManager::GetInstance()->isDevMode_BypassMapLock)
	{
		return true;
	}

	// 3. LOGIC CHẶN CỬA KHI CHƯA QUA MÀN
	StageNode* stageNode = dynamic_cast<StageNode*>(currentNode);
	if (stageNode != NULL && !stageNode->IsUnlocked())
	{
		// Lấy số thứ tự của trạm (1, 2, 3, 4...) từ enum StageNodeState
		int stageId = (int)stageNode->GetState();

		// Cờ xác nhận người chơi đang đi lùi về trạm cũ
		bool isMovingBackward = false;

		// KHAI BÁO HƯỚNG ĐI LÙI CHO TỪNG TRẠM
		switch (stageId)
		{
		case 1:
			// Ví dụ trạm 1, lùi về vị trí Start là đi xuống hoặc đi trái (Tùy map ông)
			if (nx == -1 || ny == 1) isMovingBackward = true;
			break;
		case 2:
			// TRẠM SỐ 2 ĐÂY: Hướng đi lùi duy nhất là qua Trái (nx == -1) về trạm 1.
			// Bấm Phải (nx = 1) hay Xuống (ny = 1) đều bị coi là đi tới -> isMovingBackward = false!
			if (nx == -1) isMovingBackward = true;
			break;
		case 3:
			if (nx == -1) isMovingBackward = true; // Lùi qua trái về trạm 2
			break;
		case 4:
			if (nx == -1) isMovingBackward = true; // Trạm 4 (10, 3) lùi về trái
			break;
		case 5:
			if (nx == 1) isMovingBackward = true;  // Ví dụ trạm 5 lùi qua phải
			break;
		case 6:
			if (nx == -1) isMovingBackward = true; // Ví dụ trạm 6 lùi qua trái
			break;
		default:
			// Mặc định an toàn là cho lùi qua trái
			if (nx == -1) isMovingBackward = true;
			break;
		}

		// NẾU HƯỚNG BẤM KHÔNG PHẢI HƯỚNG LÙI -> CHẶN ĐỨNG NGAY LẬP TỨC!
		if (!isMovingBackward)
		{
			return false;
		}
	}

	return true; // Cho qua
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

	if (!isMoving && CanMoveToDirection(-1, 0)) {
		SetState(MARIO_MAP_STATE_WALKING_LEFT);
		SoundManager::GetInstance()->Play("map_move");
	}
}

void MapMario::MoveRight()
{
	if (GetTickCount64() - stopTimer < 150) return;

	if (!isMoving && CanMoveToDirection(1, 0)) {
		SetState(MARIO_MAP_STATE_WALKING_RIGHT);
		SoundManager::GetInstance()->Play("map_move");

	}
}

void MapMario::MoveUp()
{
	if (GetTickCount64() - stopTimer < 150) return;

	if (!isMoving && CanMoveToDirection(0, -1)) {
		SetState(MARIO_MAP_STATE_WALKING_UP);
		SoundManager::GetInstance()->Play("map_move");

	}
}

void MapMario::MoveDown()
{
	if (GetTickCount64() - stopTimer < 150) return;

	if (!isMoving && CanMoveToDirection(0, 1)) {
		SetState(MARIO_MAP_STATE_WALKING_DOWN);
		SoundManager::GetInstance()->Play("map_move");
	}
}

void MapMario::EnterNode()
{
	if (isMoving || currentNode == NULL) return;

	StageNode* stageNode = dynamic_cast<StageNode*>(currentNode);

	if (stageNode != NULL)
	{
		if (stageNode->IsUnlocked())
		{
			return;
		}
		int sceneId = (int)stageNode->GetSceneId();

		if (sceneId > 0)
		{
			GameManager::GetInstance()->mapMarioCurrentX = this->x;
			GameManager::GetInstance()->mapMarioCurrentY = this->y;

			SceneManager::GetInstance()->InitiateSwitchScene(sceneId);
		}
	}
}