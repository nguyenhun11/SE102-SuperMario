#include <algorithm>
#include "debug.h"

#include "Mario.h"
#include "Game.h"

#include "Goomba.h"

#include "Mushroom.h"
#include "Leaf.h"
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


	if (isTakingDamage)
	{
		if (GetTickCount64() - damage_start > MARIO_HIT_TIMEOUT)
		{
			isTakingDamage = false;
			if (form != MarioForm::SMALL && form != MarioForm::SUPER)
				SetNewForm(MarioForm::SUPER);
			else if(form == MarioForm::SUPER)
				SetNewForm(MarioForm::SMALL);
			accelY = MARIO_GRAVITY;
		}
		return;
	}

	if (isSuperTransforming)
	{
		if (GetTickCount64() - transform_start > MARIO_TRANSFORM_SUPER_TIME)
		{
			untouchable = 0;
			isSuperTransforming = false;
			SetNewForm(MarioForm::SUPER);
			accelY = MARIO_GRAVITY;
		}
		return;
	}

	if (isOnPlatform)
	{
		canFly = false;
		isFlying = false;
		isFloating = false;
	}
	else
	{
		if (canFly && GetTickCount64() - fly_start > MARIO_FLYING_TIME)
		{
			canFly = false;
		}

		if (isFlying || isFloating)
		{
			if (GetTickCount64() - flap_start > 150)
			{
				isFloating = false;
				isFlying = false;
			}
			else
			{
				if (isFloating)
				{
					vy = MARIO_SLOW_FALL_SPEED;
				}
			}
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
	if (dynamic_cast<Mushroom*>(e->obj))
	{
		OnCollisionWithMushroom(e);
		return;
	}
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
	else if (dynamic_cast<Mushroom*>(e->obj))
		OnCollisionWithMushroom(e);
	else if (dynamic_cast<Leaf*>(e->obj))
		OnCollisionWithLeaf(e);
}

void Mario::OnCollisionWithGoomba(LPCOLLISIONEVENT e)
{
	Goomba* goomba = dynamic_cast<Goomba*>(e->obj);

	if (e->ny < 0)
	{
		if (goomba->GetState() != GOOMBA_STATE_DIE)
		{
			goomba->SetState(GOOMBA_STATE_DIE);

			if (IsHoldingJump)
				vy = -MARIO_HIGH_JUMP_DEFLECT_SPEED;
			else
				vy = -MARIO_JUMP_DEFLECT_SPEED;
		}
	}
	else
	{
		if (goomba->GetState() != GOOMBA_STATE_DIE)
		{
			TakeDamage();
		}
	}
}

void Mario::OnCollisionWithCoin(LPCOLLISIONEVENT e)
{
	e->obj->Delete();
	AddCoin();

	// cộng điểm ở đây
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

void Mario::OnCollisionWithMushroom(LPCOLLISIONEVENT e)
{
	Mushroom* mushroom = dynamic_cast<Mushroom*>(e->obj);
	mushroom->Delete();

	// note để nhớ bổ sung hiệu ứng bất tử chớp chớp 2.5s
	if (form == MarioForm::SMALL)
	{
		StartTransform();
	}

	// note để nhớ cộng điểm ở đây nữa
}

/// <summary>
/// / NHỚ CẬP NHẬT
/// </summary>
/// <param name="e"></param>
void Mario::OnCollisionWithLeaf(LPCOLLISIONEVENT e)
{
	e->obj->Delete();

	if (form == MarioForm::SMALL)
	{
		StartTransform();
	}
	else if (form != MarioForm::RACOON)
	{
		// NOTE để nhớ làm hiệu ứng boom
		SetNewForm(MarioForm::RACOON);
	}


	// note để nhớ cộng điểm ở đây nữa
}

#pragma endregion

// ============================== GET ID ===============================
#pragma region GET ID
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

int Mario::GetAniIdRacoon()
{
	int aniId = -1;
	if (!isOnPlatform)
	{
		if (isFlying) return ID_ANI_MARIO_RACOON_FLYING;
		if (isFloating) return ID_ANI_MARIO_RACOON_FLOATING;

		if (abs(accelX) == MARIO_ACCEL_RUN_X)
		{
			aniId = ID_ANI_MARIO_RACOON_JUMP_RUN;
		}
		else
		{
			if (vy > 0) // Lúc rớt xuống
				aniId = ID_ANI_MARIO_RACOON_FALLING;
			else // Lúc bay lên
				aniId = ID_ANI_MARIO_RACOON_JUMP_WALK;
		}
	}
	else
		if (isSitting)
		{
			aniId = ID_ANI_MARIO_RACOON_SIT;
		}
		else
			if (vx == 0)
			{
				aniId = ID_ANI_MARIO_RACOON_IDLE;
			}
			else // Đang di chuyển
			{
				if (accelX == 0)
					aniId = ID_ANI_MARIO_RACOON_WALKING;
				else if (accelX * vx < 0 && accelX != 0)
					aniId = ID_ANI_MARIO_RACOON_SKIDDING;
				else if (abs(accelX) == MARIO_ACCEL_RUN_X)
					aniId = ID_ANI_MARIO_RACOON_RUNNING;
				else if (abs(accelX) == MARIO_ACCEL_WALK_X)
					aniId = ID_ANI_MARIO_RACOON_WALKING;
			}

	if (aniId == -1) aniId = ID_ANI_MARIO_RACOON_IDLE;

	return aniId;
}


#pragma endregion 

// ============================== RENDERING ===============================
void Mario::Render()
{
	Animations* animations = Animations::GetInstance();
	int aniId = -1;
	float renderY = y;
	float renderX = x;
	if (state == static_cast<int>(MarioState::DIE))
		aniId = ID_ANI_MARIO_DIE;

	else if (isSuperTransforming)
	{
		if ((GetTickCount64() / 60) % 2 == 0)
		{
			aniId = 1100;
			renderY = y - (MARIO_BIG_BBOX_HEIGHT - MARIO_SMALL_BBOX_HEIGHT) / 2;
		}
		else
		{
			aniId = 1000;
		}
	}
	else if (isTakingDamage)
	{
		float heightDiff = (MARIO_BIG_BBOX_HEIGHT - MARIO_SMALL_BBOX_HEIGHT) / 2;

		if ((GetTickCount64() / 50) % 2 == 0)
		{
			if (form == MarioForm::RACOON) aniId = ID_ANI_MARIO_RACOON_IDLE; // nhấp ngáy form chồn
			else aniId = 1100; // nhấp nháy form to
		}
		else
		{
			aniId = 1000; 
			renderY = y + heightDiff; 
		}
	}
	else
	{
		if (form == MarioForm::SUPER)
			aniId = GetAniIdBig();
		else if (form == MarioForm::SMALL)
			aniId = GetAniIdSmall();
		else if (form == MarioForm::RACOON)
			aniId = GetAniIdRacoon();
	}

	bool isFlip = (nx > 0);
	bool shouldRender = true;
	if (untouchable == 1 && isSuperTransforming == false)
	{
		if ((GetTickCount64() / 50) % 2 == 0)
		{
			shouldRender = false;
		}
	}

	if (aniId == ID_ANI_MARIO_RACOON_SKIDDING)
	{
		renderY -= 2.0f;
	}

	if (shouldRender == true)
	{
		animations->Get(aniId)->Render(renderX, renderY, isFlip);
	}

	//RenderBoundingBox();
	
	DebugOutTitle(L"Coins: %d", coin);
}

void Mario::SetState(MarioState state)
{
	// DIE is the end state, cannot be changed! 
	if (this->state == static_cast<int>(MarioState::DIE)) return;

	if (isTakingDamage || isSuperTransforming) return;

	switch (state)
	{
	case MarioState::RUNNING:
		if (isSitting) break;

		if (!isOnPlatform)
		{
			// trên không
			// Nếu vận tốc hiện tại đang chậm, chỉ cho phép đánh võng với giới hạn tốc độ Đi bộ
			if (abs(this->vx) <= MARIO_WALKING_SPEED)
			{
				maxVx = MARIO_WALKING_SPEED * nx;
				accelX = MARIO_ACCEL_WALK_X * nx;
			}
			else
			{
				// Nếu đã có đà chạy nhanh từ trước khi nhảy, cho phép giữ nguyên giới hạn tốc độ Chạy
				maxVx = MARIO_RUNNING_SPEED * nx;
				accelX = MARIO_ACCEL_RUN_X * nx;
			}
		}
		else
		{
			// ĐANG DƯỚI ĐẤT: Bơm ga chạy hết tốc lực
			maxVx = MARIO_RUNNING_SPEED * nx;
			accelX = MARIO_ACCEL_RUN_X * nx;
		}
		break;
	case MarioState::WALKING:
		if (isSitting) break;
		maxVx = MARIO_WALKING_SPEED * nx;
		accelX = MARIO_ACCEL_WALK_X * nx;
		break;
	case MarioState::JUMP:
		if (isSitting) break;
		if (isOnPlatform)	// ddungw duoi dat
		{
			if (abs(this->vx) == MARIO_RUNNING_SPEED)
			{
				vy = -MARIO_JUMP_RUN_SPEED_Y;
				if (form == MarioForm::RACOON)
				{
					canFly = true;
					fly_start = GetTickCount64();
				}
			}
			else
				vy = -MARIO_JUMP_SPEED_Y;
		}
		else	// dang tren khong
		{
			if (form == MarioForm::RACOON)
			{
				if (canFly)
				{
					vy = -MARIO_FLYING_UP_FORCE;
					isFlying = true;
					isFloating = false;
					flap_start = GetTickCount64(); 
				}
				else if (vy > 0 && !isFloating) // Đang rớt và chưa trong chu kỳ vẫy đuôi
				{
					vy = MARIO_SLOW_FALL_SPEED;
					isFlying = false;
					isFloating = true;
					flap_start = GetTickCount64();
				}
			}
		}
		break;

	case MarioState::JUMP_RELEASE:
		if (vy < 0) vy += MARIO_JUMP_SPEED_Y / 2;
		break;

	case MarioState::SIT:
		if (isOnPlatform && form != MarioForm::SMALL)
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
	if (form==MarioForm::SUPER || form == MarioForm::RACOON)
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

void Mario::SetNewForm(MarioForm newForm)
{
	float heightDiff = (MARIO_BIG_BBOX_HEIGHT - MARIO_SMALL_BBOX_HEIGHT) / 2;


	if (this->form == MarioForm::SMALL && newForm != MarioForm::SMALL)
	{
		y -= heightDiff;
	}
	// THÊM: Nếu đang Lớn mà về Nhỏ: Phải đẩy y xuống dưới
	else if (this->form != MarioForm::SMALL && newForm == MarioForm::SMALL)
	{
		y += heightDiff;
	}

	this->form = newForm;
}

void Mario::TakeDamage()
{
	if (untouchable != 0) return;

	if (form > MarioForm::SMALL)
	{
		isTakingDamage = true;
		damage_start = GetTickCount64();

		vx = 0;
		vy = 0;
		accelX = 0;
		accelY = 0;
		StartUntouchable();
	}
	else
	{
		DebugOut(L">>> Mario DIE >>> \n");
		SetState(MarioState::DIE);
	}
}