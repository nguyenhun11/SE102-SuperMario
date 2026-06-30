#include "BoomerangBro.h"
#include "Boomerang.h"
#include "Mario.h"
#include "PlayScene.h"

BoomerangBro::BoomerangBro(float x, float y) : RespawnableEnemy(x, y)
{
	this->startX = x;
	this->ay = BOOMERANG_BRO_GRAVITY;

	SetState(static_cast<int>(BroState::WALKING));
	OnEnable(); // Tái sử dụng hàm OnEnable làm khởi tạo
}

// =================== LOGIC HỒI SINH VÀ DỌN RÁC ===================
void BoomerangBro::OnEnable()
{
	this->isFlippedVertical = false;
	this->isOnGround = false;
	this->ay = BOOMERANG_BRO_GRAVITY;
	this->action_start = GetTickCount64();
	this->jump_start = GetTickCount64();

	SetState(static_cast<int>(BroState::WALKING));

	PlayScene* scene = (PlayScene*)SceneManager::GetInstance()->GetCurrentScene();
	Mario* mario = (Mario*)scene->GetPlayer();
	if (mario != nullptr)
	{
		nx = (mario->GetX() > this->x) ? 1 : -1;
		vx = (nx > 0) ? BOOMERANG_BRO_WALKING_SPEED : -BOOMERANG_BRO_WALKING_SPEED;
	}
}

void BoomerangBro::OnExitCamera()
{
	if (isGenerated) this->Delete();
	else RespawnableEnemy::OnExitCamera();
}

// =================================================================

void BoomerangBro::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - BOOMERANG_BRO_BBOX_WIDTH / 2;
	t = y - BOOMERANG_BRO_BBOX_HEIGHT / 2;
	r = l + BOOMERANG_BRO_BBOX_WIDTH;
	b = t + BOOMERANG_BRO_BBOX_HEIGHT;
}

void BoomerangBro::ThrowBoomerang()
{
	float throwY = y - 4.0f;
	Boomerang* wpn = new Boomerang(x, throwY, nx, this);
	((PlayScene*)SceneManager::GetInstance()->GetCurrentScene())->AddObject(wpn);
}

void BoomerangBro::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (state == static_cast<int>(BroState::DIE))
	{
		vy += ay * dt;
		x += vx * dt;
		y += vy * dt;
		RespawnableEnemy::Update(dt, coObjects); // Đẩy cập nhật camera cho class cha
		return;
	}

	Mario* mario = (Mario*)((PlayScene*)SceneManager::GetInstance()->GetCurrentScene())->GetPlayer();
	if (mario != NULL && state == static_cast<int>(BroState::WALKING))
	{
		float mx, my;
		mario->GetPosition(mx, my);
		nx = (mx > this->x) ? 1 : -1;
	}

	vy += ay * dt;
	isOnGround = false;

	if (state == static_cast<int>(BroState::WALKING))
	{
		if (x <= startX - PATROL_DISTANCE) vx = BOOMERANG_BRO_WALKING_SPEED;
		else if (x >= startX + PATROL_DISTANCE) vx = -BOOMERANG_BRO_WALKING_SPEED;
	}
	else
	{
		vx = 0;
	}

	ULONGLONG now = GetTickCount64();
	switch (state)
	{
	case static_cast<int>(BroState::WALKING):
		if (now - action_start > 2500) SetState(static_cast<int>(BroState::WIND_UP_1));
		break;
	case static_cast<int>(BroState::WIND_UP_1):
		if (now - action_start > 300)
		{
			ThrowBoomerang();
			SetState(static_cast<int>(BroState::WAIT_THROW_2));
		}
		break;
	case static_cast<int>(BroState::WAIT_THROW_2):
		if (now - action_start > 1000) SetState(static_cast<int>(BroState::WIND_UP_2));
		break;
	case static_cast<int>(BroState::WIND_UP_2):
		if (now - action_start > 300)
		{
			ThrowBoomerang();
			SetState(static_cast<int>(BroState::WALKING));
		}
		break;
	}

	if (now - jump_start > 3000)
	{
		if (isOnGround && state == static_cast<int>(BroState::WALKING))
		{
			vy = -BOOMERANG_BRO_JUMP_SPEED;
		}
		jump_start = now;
	}

	RespawnableEnemy::Update(dt, coObjects);
	Collision::GetInstance()->Process(this, dt, coObjects);
}

void BoomerangBro::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
}

void BoomerangBro::OnCollisionWith(LPCOLLISIONEVENT e)
{
	if (!e->obj->IsBlocking()) return;

	if (e->ny != 0)
	{
		vy = 0;
		if (e->ny < 0) isOnGround = true;
	}
	else if (e->nx != 0)
	{
		vx = -vx;
	}
}

int BoomerangBro::IsCollidable()
{
	if (state == static_cast<int>(BroState::DIE)) return 0;
	return 1;
}

void BoomerangBro::SetState(int state)
{
	RespawnableEnemy::SetState(state);
	switch (state)
	{
	case static_cast<int>(BroState::WALKING):
		vx = (nx > 0) ? BOOMERANG_BRO_WALKING_SPEED : -BOOMERANG_BRO_WALKING_SPEED;
		action_start = GetTickCount64();
		break;
	case static_cast<int>(BroState::WIND_UP_1):
	case static_cast<int>(BroState::WAIT_THROW_2):
	case static_cast<int>(BroState::WIND_UP_2):
		vx = 0;
		action_start = GetTickCount64();
		break;
	case static_cast<int>(BroState::DIE):
		vy = -BOOMERANG_BRO_DEFLECT_SPEED;
		vx = (nx > 0) ? -0.05f : 0.05f;
		ay = BOOMERANG_BRO_GRAVITY;
		isFlippedVertical = true;
		break;
	}
}

void BoomerangBro::Render()
{
	int aniId = ID_ANI_BOOMERANG_BRO_WALKING;

	if (state == static_cast<int>(BroState::WIND_UP_1) || state == static_cast<int>(BroState::WIND_UP_2))
	{
		aniId = ID_ANI_BOOMERANG_BRO_THROWING;
	}

	bool isFlippedHorizontal = (nx > 0);
	float renderY = y + 4.5f;
	Animations::GetInstance()->Get(aniId)->Render(x, renderY, isFlippedHorizontal, isFlippedVertical);
}