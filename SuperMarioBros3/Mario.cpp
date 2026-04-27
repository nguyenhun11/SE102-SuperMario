#include <algorithm>
#include "debug.h"

#include "Mario.h"
#include "Game.h"

#include "Goomba.h"
#include "Coin.h"
#include "Portal.h"

#include "Collision.h"

void Mario::Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects)
{
	vy += accelY * dt;
	vx += accelX * dt;

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
	if (vx > 0 && maxVx > 0 && vx > maxVx) vx = maxVx;
	if (vx < 0 && maxVx < 0 && vx < maxVx) vx = maxVx;

	if (abs(vx) > abs(maxVx)) vx = maxVx;

	// reset untouchable timer if untouchable time has passed
	if ( GetTickCount64() - untouchable_start > MARIO_UNTOUCHABLE_TIME) 
	{
		untouchable_start = 0;
		untouchable = 0;
	}

	Collision::GetInstance()->Process(this, dt, coObjects);
}

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
			vy = -MARIO_JUMP_DEFLECT_SPEED;
		}
	}
	else // hit by Goomba
	{
		if (untouchable == 0)
		{
			if (goomba->GetState() != GOOMBA_STATE_DIE)
			{
				if (level > MARIO_LEVEL_SMALL)
				{
					level = MARIO_LEVEL_SMALL;
					StartUntouchable();
				}
				else
				{
					DebugOut(L">>> Mario DIE >>> \n");
					SetState(MARIO_STATE_DIE);
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
		if (vy > 0)
		{
			aniId = ID_ANI_MARIO_SUPER_FALLING;
		}
		else
		{
			if (abs(accelX) == MARIO_ACCEL_RUN_X)
			{
				aniId = ID_ANI_MARIO_SUPER_JUMP_RUN;
			}
			else
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

	if (state == MARIO_STATE_DIE)
		aniId = ID_ANI_MARIO_DIE;
	else if (level == MARIO_LEVEL_BIG)
		aniId = GetAniIdBig();
	else if (level == MARIO_LEVEL_SMALL)
		aniId = GetAniIdSmall();
	bool isFlip = (nx > 0);
	animations->Get(aniId)->Render(x, y, isFlip);

	//RenderBoundingBox();
	
	DebugOutTitle(L"Coins: %d", coin);
}

void Mario::SetState(int state)
{
	// DIE is the end state, cannot be changed! 
	if (this->state == MARIO_STATE_DIE) return; 

	currentState = state;

	switch (currentState)
	{
	case MARIO_STATE_RUNNING_RIGHT:
		if (isSitting) break;
		if (!isOnPlatform) return;
		maxVx = MARIO_RUNNING_SPEED;
		accelX = MARIO_ACCEL_RUN_X;
		nx = 1;
		break;
	case MARIO_STATE_RUNNING_LEFT:
		if (isSitting) break;
		maxVx = -MARIO_RUNNING_SPEED;
		accelX = -MARIO_ACCEL_RUN_X;
		nx = -1;
		break;
	case MARIO_STATE_WALKING_RIGHT:
		if (isSitting) break;
		maxVx = MARIO_WALKING_SPEED;
		accelX = MARIO_ACCEL_WALK_X;
		nx = 1;
		break;
	case MARIO_STATE_WALKING_LEFT:
		if (isSitting) break;
		maxVx = -MARIO_WALKING_SPEED;
		accelX = -MARIO_ACCEL_WALK_X;
		nx = -1;
		break;
	case MARIO_STATE_JUMP:
		if (isSitting) break;
		if (isOnPlatform)
		{
			if (abs(this->vx) == MARIO_RUNNING_SPEED)
				vy = -MARIO_JUMP_RUN_SPEED_Y;
			else
				vy = -MARIO_JUMP_SPEED_Y;
		}
		break;

	case MARIO_STATE_RELEASE_JUMP:
		if (vy < 0) vy += MARIO_JUMP_SPEED_Y / 2;
		break;

	case MARIO_STATE_SIT:
		if (isOnPlatform && level != MARIO_LEVEL_SMALL)
		{
			state = MARIO_STATE_IDLE;
			isSitting = true;
			vx = 0; vy = 0.0f;
			accelX = 0.0f;
			y +=MARIO_SIT_HEIGHT_ADJUST;
		}
		break;

	case MARIO_STATE_SIT_RELEASE:
		if (isSitting)
		{
			isSitting = false;
			state = MARIO_STATE_IDLE;
			y -= MARIO_SIT_HEIGHT_ADJUST;
		}
		break;

	case MARIO_STATE_IDLE:
		accelX = 0.0f;
		break;

	case MARIO_STATE_DIE:
		vy = -MARIO_JUMP_DEFLECT_SPEED;
		vx = 0;
		accelX = 0;
		break;
	}

	GameObject::SetState(state);
}

void Mario::GetBoundingBox(float &left, float &top, float &right, float &bottom)
{
	if (level==MARIO_LEVEL_BIG)
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

void Mario::SetLevel(int l)
{
	// Adjust position to avoid falling off platform
	if (this->level == MARIO_LEVEL_SMALL)
	{
		y -= (MARIO_BIG_BBOX_HEIGHT - MARIO_SMALL_BBOX_HEIGHT) / 2;
	}
	level = l;
}

