#include "KoopaTroopa.h"
#include "Koopa.h"
KoopaTroopa::KoopaTroopa(float x, float y, KoopaColor color) : Koopa(x, y, color)
{
	//SetState(KoopaState::WALKING);
	SetState(KoopaState::WING);

}

void KoopaTroopa::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (state == static_cast<int>(KoopaState::WING)) 
	{
		left = x - KOOPATROOPA_BBOX_WIDTH / 2;
		top = y - KOOPATROOPA_BBOX_HEIGHT / 2;
		right = left + KOOPATROOPA_BBOX_WIDTH;
		bottom = top + KOOPATROOPA_BBOX_HEIGHT - 4;
		
	}
	else 
	{
		Koopa::GetBoundingBox(left, top, right, bottom);
	}
}




void KoopaTroopa::OnCollisionWith(LPCOLLISIONEVENT e)
{
	Koopa::OnCollisionWith(e);

	if (state == static_cast<int>(KoopaState::WING))
	{
		if (e->ny < 0 && e->obj->IsBlocking())
		{
			if (this->color == KoopaColor::GREEN)
			{
				vy = -0.35f;
			}
		}
	}
}

//void KoopaTroopa::OnCollisionWith(LPCOLLISIONEVENT e)
//{
//	if (state == static_cast<int>(KoopaState::WING))
//	{
//		if (e->ny < 0 && (dynamic_cast<Koopa*>(e->obj) != nullptr || dynamic_cast<Goomba*>(e->obj) != nullptr))
//		{
//			return; 
//		}
//	}
//
//	Koopa::OnCollisionWith(e);
//
//	if (state == static_cast<int>(KoopaState::WING))
//	{
//		if (e->ny < 0 && e->obj->IsBlocking())
//		{
//			if (this->color == KoopaColor::GREEN)
//			{
//				vy = -0.35f; // Nảy lên
//			}
//		}
//	}
//}

void KoopaTroopa::OnEnable()
{
	//this->state = static_cast<int>(KoopaState::WING);
	SetState(KoopaState::WING);
	this->isHeld = false;
	this->isFlippedVertical = false;

	if (this->color == KoopaColor::GREEN)
	{
		ay = KOOPA_GRAVITY;
		vx = nx * KOOPA_WALKING_SPEED;
		vy = -0.15f;
	}
	else if (this->color == KoopaColor::RED)
	{
		ay = 0;
		vx = 0;
		vy = -0.04f;
	}

	PlayScene* scene = (PlayScene*)SceneManager::GetInstance()->GetCurrentScene();
	Mario* mario = (Mario*)scene->GetPlayer();
	if (mario != nullptr)
	{
		if (color == KoopaColor::GREEN)
		{
			nx = (mario->GetX() > this->x) ? 1 : -1;
			vx = nx * KOOPA_WALKING_SPEED;
		}
	}
	else
	{
		nx = -1;
		vx = -KOOPA_WALKING_SPEED;
	}
}


void KoopaTroopa::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (isHeld)
	{
		Koopa::Update(dt, coObjects);
		return;
	}

	vy += ay * dt;
	vx += ax * dt;


	if (state == static_cast<int>(KoopaState::WING))
	{
		if (this->color == KoopaColor::RED)
		{
			ay = 0;
			vx = 0; 
			float startX, startY;
			respawnPoint->GetPosition(startX, startY);

			float limitTop = startY - 40.0f;
			float limitBottom = startY + 40.0f;

			if (y <= limitTop)
			{
				y = limitTop;
				vy = 0.04f; 
			}
			else if (y >= limitBottom)
			{
				y = limitBottom;
				vy = -0.04f; 
			}
		}
	}

	
	if ((this->state == static_cast<int>(KoopaState::SHELL)) && (GetTickCount64() - die_start > KOOPA_DIE_TIMEOUT - 200))
	{
		SetState(KoopaState::SHAKING);
	}
	if ((this->state == static_cast<int>(KoopaState::SHAKING)) && (GetTickCount64() - die_start > KOOPA_DIE_TIMEOUT))
	{
		SetState(KoopaState::WALKING);
		return;
	}

	GameObject::Update(dt, coObjects);
	Collision::GetInstance()->Process(this, dt, coObjects);
}


void KoopaTroopa::Render()
{
	int aniId = -1;
	float renderX = x;
	float renderY = y;

	if (state == static_cast<int>(KoopaState::WING))
	{
		if (this->color == KoopaColor::GREEN)
			aniId = ID_ANI_KOOPA_GREEN_WING;
		else
			aniId = ID_ANI_KOOPA_RED_WING;

		isFlipped = (nx > 0);

		Animations::GetInstance()->Get(aniId)->Render(renderX, renderY, isFlipped);
		return;
	}
	else
		Koopa::Render();
}


void KoopaTroopa::SetState(KoopaState state)
{
	KoopaState oldState = static_cast<KoopaState>(this->state);
	Koopa::SetState(state);

	if (oldState == KoopaState::WING && this->state == static_cast<int>(KoopaState::WALKING))
	{
		ay = KOOPA_GRAVITY;
		vy = 0;
	}

	if (this->state == static_cast<int>(KoopaState::WING))
	{
		if (this->color == KoopaColor::GREEN)
		{
			ay = KOOPA_FLYING_GRAVITY;
			vx = nx * KOOPA_FLYING_SPEED;
			vy = -0.15f;
		}
		else if (this->color == KoopaColor::RED)
		{
			ay = 0; 
			vx = 0;
			vy = -0.04f; 
		}
	}
}