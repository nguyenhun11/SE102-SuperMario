#include "BoomerangBro.h"
#include "Boomerang.h"
#include "Mario.h"
#include "PlayScene.h"
#include "SoundManager.h"

BoomerangBro::BoomerangBro(float x, float y) : RespawnableEnemy(x, y)
{
	this->startX = x;
	this->ay = BOOMERANG_BRO_GRAVITY;
	OnEnable();
}

void BoomerangBro::OnEnable()
{
	this->isFlippedVertical = false;
	this->ay = BOOMERANG_BRO_GRAVITY;

	// Reset logic ném
	this->throwCount = 0;
	this->throwCooldown = 2000; // Sinh ra sau 2 giây mới ném phát đầu
	this->throwStart = GetTickCount64();
	this->isThrowing = false;

	this->jumpCooldown = 3000; // Sinh ra sau 3 giây mới nhảy phát đầu
	this->jumpStart = GetTickCount64();

	// Hướng mặt ban đầu
	PlayScene* scene = (PlayScene*)SceneManager::GetInstance()->GetCurrentScene();
	Mario* mario = (Mario*)scene->GetPlayer();
	if (mario != nullptr) {
		nx = (mario->GetX() > this->x) ? 1 : -1;
	}
	else {
		nx = -1;
	}

	this->vx = BOOMERANG_BRO_WALKING_SPEED;
	SetState(static_cast<int>(BroState::ALIVE));

	this->activeBoomerangs = 0;
	this->boomerangSoundTimer = 0;
}

void BoomerangBro::OnExitCamera()
{
	RespawnableEnemy::OnExitCamera();
}


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

	this->activeBoomerangs++;
}

void BoomerangBro::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (state == static_cast<int>(BroState::DIE))
	{
		vy += ay * dt;
		x += vx * dt;
		y += vy * dt;
		RespawnableEnemy::Update(dt, coObjects);
		return;
	}

	// 1. LUÔN NHÌN VỀ PHÍA MARIO
	Mario* mario = (Mario*)((PlayScene*)SceneManager::GetInstance()->GetCurrentScene())->GetPlayer();
	if (mario != NULL)
	{
		float mx, my;
		mario->GetPosition(mx, my);
		nx = (mx > this->x) ? 1 : -1;
	}

	vy += ay * dt;
	ULONGLONG now = GetTickCount64();

	// -------------------------------------------------------------
	// HỆ THỐNG 1: DI CHUYỂN PING-PONG (Không phụ thuộc Animation)
	// -------------------------------------------------------------
	if (x <= startX - PATROL_RANGE)
	{
		x = startX - PATROL_RANGE; // Ép biên chống trôi
		vx = BOOMERANG_BRO_WALKING_SPEED;
	}
	else if (x >= startX + PATROL_RANGE)
	{
		x = startX + PATROL_RANGE;
		vx = -BOOMERANG_BRO_WALKING_SPEED;
	}

	// -------------------------------------------------------------
	// HỆ THỐNG 2: CHU KỲ NÉM 2 NHỊP (Asynchronous)
	// -------------------------------------------------------------
	if (now - throwStart > throwCooldown)
	{
		ThrowBoomerang();
		isThrowing = true;
		throwAniStart = now;

		if (throwCount == 0) // Vừa ném phát 1 xong
		{
			throwCount = 1;
			throwCooldown = 1500; // Đợi 1.5 giây ném phát 2
		}
		else // Vừa ném phát 2 xong
		{
			throwCount = 0;
			throwCooldown = 3000; // Đợi 3 giây để nạp lại chu kỳ mới
		}

		throwStart = now;
	}

	// Trả lại hình dáng đi bộ sau khi giơ tay 0.3s
	if (isThrowing && now - throwAniStart > 300)
	{
		isThrowing = false;
	}

	if (isOnGround && (now - jumpStart > jumpCooldown))
	{
		vy = -BOOMERANG_BRO_JUMP_SPEED;
		isOnGround = false;
		jumpStart = now;

		jumpCooldown = 2000 + rand() % 2000;
	}

	if (isIdleAtEdge)
	{
		vx = 0; // Đứng yên
		if (now - idleStart > 500) // Dừng 0.5 giây ở biên
		{
			isIdleAtEdge = false; // Hết giờ nghỉ, đi tiếp
			vx = (x < startX) ? BOOMERANG_BRO_WALKING_SPEED : -BOOMERANG_BRO_WALKING_SPEED;
		}
	}
	else
	{
		// Đang đi bộ
		if (x <= startX - PATROL_RANGE)
		{
			x = startX - PATROL_RANGE;
			isIdleAtEdge = true; // Bắt đầu dừng
			idleStart = now;
		}
		else if (x >= startX + PATROL_RANGE)
		{
			x = startX + PATROL_RANGE;
			isIdleAtEdge = true; // Bắt đầu dừng
			idleStart = now;
		}
	}

	if (activeBoomerangs > 0)
	{
		// Nếu có ít nhất 1 cái đang bay, đếm ngược và phát tiếng
		boomerangSoundTimer -= (float)dt;
		if (boomerangSoundTimer <= 0)
		{
			SoundManager::GetInstance()->Play("boomerang");
			boomerangSoundTimer = 150.0f;
		}
	}
	else
	{
		// Nếu tất cả đã thu hồi, tắt đồng hồ
		boomerangSoundTimer = 0;
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
	if (state == static_cast<int>(BroState::DIE))
	{
		vy = -BOOMERANG_BRO_DEFLECT_SPEED;
		vx = (nx > 0) ? -0.05f : 0.05f;
		ay = BOOMERANG_BRO_GRAVITY;
		isFlippedVertical = true;
	}
}

void BoomerangBro::Render()
{
	int aniId = isThrowing ? ID_ANI_BOOMERANG_BRO_THROWING : ID_ANI_BOOMERANG_BRO_WALKING;
	bool isFlippedHorizontal = (nx > 0);

	float renderY = y + 4.5f;
	Animations::GetInstance()->Get(aniId)->Render(x, renderY, isFlippedHorizontal, isFlippedVertical);
}