#include <algorithm>
#include "debug.h"

#include "Mario.h"
#include "Game.h"

#include "Goomba.h"
#include "Coin.h"
#include "Portal.h"
#include "QuestionBlock.h"

#include "Collision.h"

void Mario::Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects)
{
	if (state == static_cast<int>(MarioState::DIE))
	{
		// Kiểm tra xem đã hết thời gian timeout chưa
		if (GetTickCount64() - die_start > MARIO_DIE_TIMEOUT && accelY == 0)
		{
			// nảy mario lên
			vy = -MARIO_DIE_BOUNCE_FORCE;
			accelY = MARIO_DIE_GRAVITY;
		}

		vy += accelY * dt;
		y += vy * dt;
		x += vx * dt;

		return; 
	}

	if (state == static_cast<int>(MarioState::HIT))
	{
		if (GetTickCount64() - hit_start < MARIO_HIT_TIMEOUT)
		{
			return;
		}
	}

	vy += accelY * dt;
	vx += accelX * dt;

	float effectiveAccel = accelX;
	if (accelX * vx < 0)
	{
		if (accelX > 0)
		{
			effectiveAccel = MARIO_ACCEL_SKID;
		}
		else
		{
			effectiveAccel = -MARIO_ACCEL_SKID;
		}
	}

	vx += effectiveAccel * dt;

	if (accelX == 0 && vx != 0)
	{
		if (vx > 0)	// mario đang di chuyển về bên phải
		{
			vx -= MARIO_DECCEL_WALK_X * dt;
			if (vx < 0) vx = 0;
		}
		else		// mario dang di chuyen ve ben trai
		{
			vx += MARIO_DECCEL_WALK_X * dt;
			if (vx > 0) vx = 0;
		}
	}

	// gioi han toc do di chuyen
	if (accelX * vx > 0)
	{
		if (vx > 0 && maxVx > 0 && vx > maxVx) vx = maxVx;
		if (vx < 0 && maxVx < 0 && vx < maxVx) vx = maxVx;
	}

	// reset untouchable timer if untouchable time has passed
	if ( GetTickCount64() - untouchable_start > MARIO_UNTOUCHABLE_TIME) 
	{
		untouchable_start = 0;
		untouchable = 0;
	}

	Collision::GetInstance()->Process(this, dt, coObjects);
}

#pragma region COLLISION

void Mario::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
	isOnPlatform = false;
}

void Mario::OnCollisionWith(LPCOLLISIONEVENT e)
{
	if (e->ny != 0 && e->obj->IsBlocking())
	{
		vy = 0;
		if (e->ny < 0) isOnPlatform = true;
	}
	else 
	if (e->nx != 0 && e->obj->IsBlocking())
	{
		vx = 0;
	}

	if (dynamic_cast<Goomba*>(e->obj))
		OnCollisionWithGoomba(e);
	else if (dynamic_cast<Coin*>(e->obj))
		OnCollisionWithCoin(e);
	else if (dynamic_cast<Portal*>(e->obj))
		OnCollisionWithPortal(e);
	else if (dynamic_cast<QuestionBlock*>(e->obj))
		OnCollisionWithQuestionBlock(e);
}

void Mario::OnCollisionWithGoomba(LPCOLLISIONEVENT e)
{
	Goomba* goomba = dynamic_cast<Goomba*>(e->obj);

	// jump on top >> kill Goomba and deflect a bit 
	if (e->ny < 0)
	{
		if (goomba->GetState() != GOOMBA_STATE_DIE)
		{
			goomba->SetState(GOOMBA_STATE_DIE);

			// nếu người chơi giữ phím nhảy thì nảy lên cao hơn
			if (IsHoldingJump)
			{
				vy = -MARIO_HIGH_JUMP_DEFLECT_SPEED;
			}
			// nếu người chơi không giữ phím nhảy thì nảy thấp hơn
			else
			{
				vy = -MARIO_JUMP_DEFLECT_SPEED;
			}
		}
	}
	else // hit by Goomba
	{
		if (untouchable == 0)
		{
			if (goomba->GetState() != GOOMBA_STATE_DIE)
			{
				hit_start = GetTickCount64();
				if (level > MarioForm::SMALL)
				{
					//SetState(MarioState::HIT);
					level = MarioForm::SMALL;
					StartUntouchable();
				}
				else
				{
					DebugOut(L">>> Mario DIE >>> \n");
					SetState(MarioState::DIE);
				}
			}
		}
	}
}

void Mario::OnCollisionWithCoin(LPCOLLISIONEVENT e)
{
	e->obj->Delete();
	coin++;
}

void Mario::OnCollisionWithPortal(LPCOLLISIONEVENT e)
{
	Portal* p = (Portal*)e->obj;
	SceneManager::GetInstance()->InitiateSwitchScene(p->GetSceneId());
}

void Mario::OnCollisionWithQuestionBlock(LPCOLLISIONEVENT e)
{
	QuestionBlock* qb = dynamic_cast<QuestionBlock*>(e->obj);
	if (e->ny > 0)
	{
		if (qb->GetCurrentState() == QuestionBlockState::ACTIVE)
		{
			qb->SetState(QuestionBlockState::BOUNCING);
		}
	}
}

#pragma endregion

//
// Get animation ID for small Mario
//
int Mario::GetAniIdSmall()
{
	int aniId = -1;
	if (!isOnPlatform)	// mario tren khong
	{
		if (abs(accelX) == MARIO_ACCEL_RUN_X)
		{
			aniId = ID_ANI_MARIO_SMALL_JUMP_RUN;
		}
		else
		{
			aniId = ID_ANI_MARIO_SMALL_JUMP_WALK;
		}
	}
	else		// mario dung tren mat đất
		if (isSitting)
		{
			aniId = ID_ANI_MARIO_SUPER_SIT;
		}
		else
		{
			if (vx == 0)	// mario đứng yên
			{
				aniId = ID_ANI_MARIO_SMALL_IDLE;
			}
			else	// vx != 0 <=> Mario dang di chuyen
			{
				if (accelX == 0)
					aniId = ID_ANI_MARIO_SMALL_WALKING;
				else if (vx * accelX < 0 && accelX != 0)	// Nguoi choi tha nut di chuyen --> Skidding
					aniId = ID_ANI_MARIO_SMALL_SKIDDING;
				else if (abs(accelX) == MARIO_ACCEL_RUN_X)
					aniId = ID_ANI_MARIO_SMALL_RUNNING;
				else if (abs(accelX) == MARIO_ACCEL_WALK_X)
					aniId = ID_ANI_MARIO_SMALL_WALKING;
			}
		}


	if (aniId == -1) aniId = ID_ANI_MARIO_SMALL_IDLE;

	return aniId;
}


//
// Get animdation ID for big Mario
//
int Mario::GetAniIdBig()
{
	int aniId = -1;
	if (!isOnPlatform)
	{
		if (abs(accelX) == MARIO_ACCEL_RUN_X)
		{
			aniId = ID_ANI_MARIO_SUPER_JUMP_RUN;
		}
		else
		{
			if (vy > 0) // Lúc rớt xuống
			{
				aniId = ID_ANI_MARIO_SUPER_FALLING;
			}
			else // Lúc bay lên
			{
				aniId = ID_ANI_MARIO_SUPER_JUMP_WALK;
			}
		}
	}
	else
		if (isSitting)
		{
			aniId = ID_ANI_MARIO_SUPER_SIT;
		}
		else
			if (vx == 0)
			{
				aniId = ID_ANI_MARIO_SUPER_IDLE;
			}
			else // vx < 0
			{
				if (accelX == 0)
					aniId = ID_ANI_MARIO_SUPER_WALKING;
				if (accelX * vx < 0 && accelX != 0)
					aniId = ID_ANI_MARIO_SUPER_SKIDDING;
				else if (abs(accelX) == MARIO_ACCEL_RUN_X)
					aniId = ID_ANI_MARIO_SUPER_RUNNING;
				else if (abs(accelX) == MARIO_ACCEL_WALK_X)
					aniId = ID_ANI_MARIO_SUPER_WALKING;
			}

	if (aniId == -1) aniId = ID_ANI_MARIO_SUPER_IDLE;

	return aniId;
}

void Mario::Render()
{
	Animations* animations = Animations::GetInstance();
	int aniId = -1;

	if (state == static_cast<int>(MarioState::DIE))
		aniId = ID_ANI_MARIO_DIE;
	else if (level == MarioForm::SUPER)
		aniId = GetAniIdBig();
	else if (level == MarioForm::SMALL)
		aniId = GetAniIdSmall();
	bool isFlip = (nx > 0);
	animations->Get(aniId)->Render(x, y, isFlip);

	//RenderBoundingBox();
	
	DebugOutTitle(L"Coins: %d", coin);
}

void Mario::SetState(MarioState state)
{
	// DIE is the end state, cannot be changed! 
	if (this->state == static_cast<int>(MarioState::DIE)) return;


	switch (state)
	{
	case MarioState::RUNNING:
		if (isSitting) break;
		if (!isOnPlatform) return;
		maxVx = MARIO_RUNNING_SPEED * nx;
		accelX = MARIO_ACCEL_RUN_X * nx;
		break;
	case MarioState::WALKING:
		if (isSitting) break;
		maxVx = MARIO_WALKING_SPEED * nx;
		accelX = MARIO_ACCEL_WALK_X * nx;
		break;
	case MarioState::JUMP:
		if (isSitting) break;
		if (isOnPlatform)
		{
			if (abs(this->vx) == MARIO_RUNNING_SPEED)
				vy = -MARIO_JUMP_RUN_SPEED_Y;
			else
				vy = -MARIO_JUMP_SPEED_Y;
		}
		break;

	case MarioState::JUMP_RELEASE:
		if (vy < 0) vy += MARIO_JUMP_SPEED_Y / 2;
		break;

	case MarioState::SIT:
		if (isOnPlatform && level != MarioForm::SMALL)
		{
			state = MarioState::IDLE;
			isSitting = true;
			vy = 0.0f;
			accelX = 0.0f;
			y +=MARIO_SIT_HEIGHT_ADJUST;
		}
		break;

	case MarioState::SIT_RELEASE:
		if (isSitting)
		{
			isSitting = false;
			state = MarioState::IDLE;
			y -= MARIO_SIT_HEIGHT_ADJUST;
		}
		break;

	case MarioState::IDLE:
		accelX = 0.0f;
		break;

	case MarioState::HIT:
		vx = 0;
		vy = 0;
		accelX = 0;
		accelY = 0;
		break;

	case MarioState::DIE:
		die_start = GetTickCount64(); 
		vx = 0;
		vy = 0;     
		accelX = 0;
		accelY = 0;  
		break;
	}

	GameObject::SetState(static_cast<int>(state));
}

void Mario::GetBoundingBox(float &left, float &top, float &right, float &bottom)
{
	if (level==MarioForm::SUPER)
	{
		if (isSitting)
		{
			left = x - MARIO_BIG_SITTING_BBOX_WIDTH / 2;
			top = y - MARIO_BIG_SITTING_BBOX_HEIGHT / 2;
			right = left + MARIO_BIG_SITTING_BBOX_WIDTH;
			bottom = top + MARIO_BIG_SITTING_BBOX_HEIGHT;
		}
		else 
		{
			left = x - MARIO_BIG_BBOX_WIDTH/2;
			top = y - MARIO_BIG_BBOX_HEIGHT/2;
			right = left + MARIO_BIG_BBOX_WIDTH;
			bottom = top + MARIO_BIG_BBOX_HEIGHT;
		}
	}
	else
	{
		left = x - MARIO_SMALL_BBOX_WIDTH/2;
		top = y - MARIO_SMALL_BBOX_HEIGHT/2;
		right = left + MARIO_SMALL_BBOX_WIDTH;
		bottom = top + MARIO_SMALL_BBOX_HEIGHT;
	}
}

void Mario::SetLevel(MarioForm form)
{
	// Adjust position to avoid falling off platform
	if (this->level == MarioForm::SMALL)
	{
		y -= (MARIO_BIG_BBOX_HEIGHT - MARIO_SMALL_BBOX_HEIGHT) / 2;
	}
	level = form;
}

