#include "Goomba.h"
#include "Camera.h"
#include "PlayScene.h"
#include "GameGlobal.h"
#include "Slope.h"
#include "HitEffect.h"

Goomba::Goomba(float x, float y) : RespawnableEnemy(x, y)
{
	this->ax = 0;
	this->ay = GOOMBA_GRAVITY;
	die_start = -1;
	this->zIndex = 8;

	SetState(GoombaState::WALKING);
	OnEnable();
}

void Goomba::OnEnable()
{
	SetState(GoombaState::WALKING);
	isFlippedVertical = false;
	PlayScene* scene = (PlayScene*)SceneManager::GetInstance()->GetCurrentScene();
	Mario* mario = (Mario*)scene->GetPlayer();

	if (mario != nullptr)
	{
		nx = (mario->GetX() > this->x) ? 1 : -1;
		//nx = -1;
		vx = nx * GOOMBA_WALKING_SPEED;
	}
	else
	{
		nx = -1;
		vx = -GOOMBA_WALKING_SPEED;
	}
}

void Goomba::OnExitCamera()
{
	if (isGenerated)
	{
		this->Delete();
	}
	else
	{
		RespawnableEnemy::OnExitCamera();
	}
}

void Goomba::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (state == static_cast<int>(GoombaState::DIE))
	{
		left = x - GOOMBA_BBOX_WIDTH / 2;
		top = y - GOOMBA_BBOX_HEIGHT_DIE / 2;
		right = left + GOOMBA_BBOX_WIDTH;
		bottom = top + GOOMBA_BBOX_HEIGHT_DIE;
	}
	else
	{
		left = x - GOOMBA_BBOX_WIDTH / 2 + 0.5f;
		top = y - GOOMBA_BBOX_HEIGHT / 2;
		right = left + GOOMBA_BBOX_WIDTH - 1.0f;
		bottom = top + GOOMBA_BBOX_HEIGHT - 1;
	}
}

void Goomba::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
};

void Goomba::OnCollisionWith(LPCOLLISIONEVENT e)
{
	if (!e->obj->IsBlocking()) return;
	if (dynamic_cast<Goomba*>(e->obj)) return;

	if (e->ny != 0)
	{
		vy = 0;
	}
	else if (e->nx != 0)
	{
		vx = -vx;
		nx = -nx;
	}
}

void Goomba::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (isEmerging)
	{
		x += vx * dt;
		y += vy * dt;

		if ((nx > 0 && x >= emergeTargetX) || (nx < 0 && x <= emergeTargetX))
		{
			isEmerging = false;      // Tắt cờ chui cống
			ay = GOOMBA_GRAVITY;     // MỞ LẠI TRỌNG LỰC!
		}

		return;
	}

	if (state == static_cast<int>(GoombaState::DIE) && die_start > 0)
	{
		if (GetTickCount64() - die_start > GOOMBA_DIE_TIMEOUT)
		{
			//Delete();
			SetActive(false);
			return;
		}
	}
	else
	{
		vy += ay * dt;
		vx += ax * dt;
	}

	RespawnableEnemy::Update(dt, coObjects);
	Collision::GetInstance()->Process(this, dt, coObjects);

	HandleSlope(dt, coObjects);
}


void Goomba::Render()
{
	float renderX = x;
	float renderY = y;
	int aniId = ID_ANI_GOOMBA_WALKING;
	if (state == static_cast<int>(GoombaState::DIE))
	{
		aniId = ID_ANI_GOOMBA_DIE;
		renderY -= 3.0f;
	}

	Animations::GetInstance()->Get(aniId)->Render(renderX, renderY, false, isFlippedVertical);
}

void Goomba::SetState(GoombaState state)
{
	RespawnableEnemy::SetState(static_cast<int>(state));
	switch (state)
	{
	case GoombaState::DIE:
		die_start = GetTickCount64();
		y += (GOOMBA_BBOX_HEIGHT - GOOMBA_BBOX_HEIGHT_DIE) / 2;
		vx = 0;
		vy = 0;
		ay = 0;
		break;
	case GoombaState::BOUNCE:
		this->ay = GOOMBA_GRAVITY;
		this->vy = -0.35f;
		this->vx = this->nx * 0.05f;
		this->isFlippedVertical = true;
		break;
	case GoombaState::WALKING:
		ay = GOOMBA_GRAVITY;
		vx = nx * GOOMBA_WALKING_SPEED;
		break;
	}
}
//
//void Goomba::UpdateActivationBoundary()
//{
//	float camX, camY;
//
//	Camera::GetInstance()->GetCamPos(camX, camY);
//
//	activationBoundary = camX + GameGlobal::GetWidth();
//}
//
//bool Goomba::IsInsideActivationBoundary()
//{
//	return x <= activationBoundary;
//}
//
//void Goomba::CheckActivationStatus()
//{
//	if (isActivated)
//		return;
//
//	UpdateActivationBoundary();
//
//	if (IsInsideActivationBoundary())
//	{
//		isActivated = true;
//		vx = -GOOMBA_WALKING_SPEED;
//	}
//}

void Goomba::HandleSlope(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (state == static_cast<int>(MushroomState::SPAWNING)) return;

	float l, t, r, b;
	GetBoundingBox(l, t, r, b);
	float mushroomBottomY = b;
	float bboxHeight = b - t;

	bool foundSlope = false;

	for (size_t i = 0; i < coObjects->size(); i++)
	{
		LPGAMEOBJECT obj = coObjects->at(i);
		if (dynamic_cast<Slope*>(obj)) // Kiểm tra nếu object là dốc
		{
			Slope* slope = dynamic_cast<Slope*>(obj);
			float sl, st, sr, sb;
			slope->GetBoundingBox(sl, st, sr, sb);

			float mushroomCenterX = x;

			if (mushroomCenterX >= sl && mushroomCenterX <= sr && mushroomBottomY >= st && mushroomBottomY <= sb)
			{
				float expectedY = slope->GetSurfaceY(mushroomCenterX);
				float epsilon = max(4.0f, vy * dt);
				if (mushroomBottomY >= expectedY - epsilon && vy >= 0)
				{
					y = expectedY - bboxHeight / 2;
					vy = 0;
					foundSlope = true;
				}
			}
		}
	}

	isOnSlope = foundSlope;
}