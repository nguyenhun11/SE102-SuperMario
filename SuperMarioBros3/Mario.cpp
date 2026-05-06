#include <algorithm>
#include "debug.h"

#include "Mario.h"
#include "Game.h"
#include "PlayScene.h"

#include "Goomba.h"

#include "Item.h"
#include "ScoreEffect.h"
#include "OneUpEffect.h"
#include "Slope.h"

#include "Portal.h"
#include "QuestionBlock.h"

#include "Collision.h"

void Mario::Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects)
{
	HandleDying(dt, coObjects);
	HandleTakingDamage(dt, coObjects);
	HandleTransform(dt, coObjects);
	HandleSpinning(dt, coObjects);

	if (isOnPlatform && vy >= 0) // mario đang đứng trên platform
	{
		canFly = false;
		isFlying = false;
		isFloating = false;
	}
	else   // mario đứng trên ko trung
	{
		if (canFly && GetTickCount64() - fly_start > MARIO_FLYING_TIME)
		{
			canFly = false;
			pmeter = 0;
		}

		if (isFlying || isFloating)
		{
			if (GetTickCount64() - flap_start > MARIO_FLOATING_TIME)
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
	float effectiveMaxVx = maxVx;

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

	///// ------ vật lý của joke
	HandleSlopePhysics(dt, coObjects);

	if (isOnSlope && !isSitting)
	{
		if (slopeDirection * nx > 0)	/// LÊN  JOKE
		{
			// giới hạn tốc tối đa
			effectiveMaxVx = MARIO_WALKING_SPEED * 0.6f;

			// triệt tiêu quán tính
			if (abs(vx) > effectiveMaxVx)
			{
				vx -= (vx > 0 ? 1 : -1) * MARIO_DECCEL_RUN_X * dt * 2.0f;
			}

			// giảm gia tốc đẩy
			effectiveAccel *= 0.1f;
		}
		else if (slopeDirection * nx < 0) // XUỐNG JOKE
		{
			//effectiveMaxVx = MARIO_WALKING_SPEED * 1.05f;
			//effectiveAccel *= 1.05f; 
		}
	}

	vx += effectiveAccel * dt;
	if (effectiveAccel * vx > 0)
	{
		if (vx > 0 && effectiveMaxVx > 0 && vx > effectiveMaxVx) vx = effectiveMaxVx;
		if (vx < 0 && effectiveMaxVx < 0 && vx < effectiveMaxVx) vx = effectiveMaxVx;
	}

	////

	/// -- P meter
	HandlePMeter(dt, coObjects);

	if (isOnPlatform)
	{
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
	}

	//// gioi han toc do di chuyen
	//if (accelX * vx > 0)
	//{
	//	if (vx > 0 && maxVx > 0 && vx > maxVx) vx = maxVx;
	//	if (vx < 0 && maxVx < 0 && vx < maxVx) vx = maxVx;
	//}

	// reset untouchable timer if untouchable time has passed
	if ( GetTickCount64() - untouchable_start > MARIO_UNTOUCHABLE_TIME) 
	{
		untouchable_start = 0;
		untouchable = 0;
	}

	Collision::GetInstance()->Process(this, dt, coObjects);
	HandleSlope(dt, coObjects);
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
	else if (dynamic_cast<Brick*>(e->obj))
		OnCollisionWithBrick(e);
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
	// coongj dieemr
	Mushroom* mushroom = dynamic_cast<Mushroom*>(e->obj);
	PlayScene* scene = dynamic_cast<PlayScene*>(SceneManager::GetInstance()->GetCurrentScene());

	if (mushroom == dynamic_cast<OneUpMushroom*>(e->obj))		// nếu là nấm 1 up
	{
		// Hiệu ứng 1UP
		OneUpEffect* effect = new OneUpEffect(mushroom->GetX(), mushroom->GetY());
		scene->AddObject(effect);
		// cộng mạng
		GameManager::GetInstance()->AddLife(1);
	}
	// nếu là nấm bình thường, không phải nấm 1 up
	else
	{
		if (form == MarioForm::SMALL)
		{
			StartTransform();
		}

		// hiệu ứng điểm
		ScoreEffect* scoreEff = new ScoreEffect(mushroom->GetX(), mushroom->GetY(), Score::ONE_THOUSAND);
		scene->AddObject(scoreEff);
		// coongj điểm
		AddScore(1000);
	}

	mushroom->Delete();
}

void Mario::OnCollisionWithBrick(LPCOLLISIONEVENT e)
{
	Brick* brick = dynamic_cast<Brick*>(e->obj);

	if (e->ny > 0)
	{
		if (brick->GetCurrentState() == BrickState::ACTIVE)
		{
			if (brick->GetContainedItem() != BrickItem::NONE)
			{
				// Có đồ: Bất kể Mario dạng nào cũng chỉ nảy lên để lấy đồ
				brick->SetState(BrickState::BOUNCING);
			}
			else
			{
				// Không có đồ, xét theo sức mạnh của Mario
				if (this->GetCurrentForm() == MarioForm::SMALL)
				{
					brick->SetState(BrickState::BOUNCING); // Yếu thì chỉ nảy
				}
				else
				{
					brick->Break(); // Mạnh (To, Đuôi, Lửa) thì đập vỡ
				}
			}
		}
	}
}

/// <summary>
/// / NHỚ CẬP NHẬT
/// </summary>
/// <param name="e"></param>
void Mario::OnCollisionWithLeaf(LPCOLLISIONEVENT e)
{
	Leaf* leaf = dynamic_cast<Leaf*>(e->obj);
	PlayScene* scene = dynamic_cast<PlayScene*>(SceneManager::GetInstance()->GetCurrentScene());
	ScoreEffect* scoreEff = new ScoreEffect(leaf->GetX(), leaf->GetY(), Score::ONE_THOUSAND);
	scene->AddObject(scoreEff);
	AddScore(1000);
	leaf->Delete();
	if (form == MarioForm::SMALL)
	{
		StartTransform();
	}
	else if (form != MarioForm::RACOON)
	{
		// NOTE để nhớ làm hiệu ứng boom
		StartPoofTransform(MarioForm::RACOON);
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
		if (pmeter == MARIO_PMETER_MAX)
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
				{
					if (pmeter == MARIO_PMETER_MAX) aniId = ID_ANI_MARIO_SMALL_RUNNING;
					else aniId = ID_ANI_MARIO_SMALL_WALKING;
				}
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
		if (pmeter == MARIO_PMETER_MAX)
		{
			aniId = ID_ANI_MARIO_SUPER_JUMP_RUN;
		}
		else
		{
			if (vy > 0) aniId = ID_ANI_MARIO_SUPER_FALLING;
			else aniId = ID_ANI_MARIO_SUPER_JUMP_WALK;
		}
	}
	else
		if (isSitting)
		{
			if (isOnSlope)
			{
				aniId = ID_ANI_MARIO_SUPER_SLIDING;
			}
			else
			{
				aniId = ID_ANI_MARIO_SUPER_SIT;
			}
		}
		else
			if (vx == 0)
			{
				aniId = ID_ANI_MARIO_SUPER_IDLE;
			}
			else // vx < 0
			{
				if (accelX == 0) aniId = ID_ANI_MARIO_SUPER_WALKING;
				if (accelX * vx < 0 && accelX != 0) aniId = ID_ANI_MARIO_SUPER_SKIDDING;
				else if (abs(accelX) == MARIO_ACCEL_RUN_X)
				{
					if (pmeter == MARIO_PMETER_MAX) aniId = ID_ANI_MARIO_SUPER_RUNNING;
					else aniId = ID_ANI_MARIO_SUPER_WALKING;
				}
				else if (abs(accelX) == MARIO_ACCEL_WALK_X)
					aniId = ID_ANI_MARIO_SUPER_WALKING;
			}

	if (aniId == -1) aniId = ID_ANI_MARIO_SUPER_IDLE;

	return aniId;
}

int Mario::GetAniIdRacoon()
{
	if (isSpinning && !isSitting) return ID_ANI_MARIO_RACOON_SPIN;
	int aniId = -1;
	if (!isOnPlatform)
	{
		if (isFlying) return ID_ANI_MARIO_RACOON_FLYING;
		if (isFloating) return ID_ANI_MARIO_RACOON_FLOATING;

		if (pmeter == MARIO_PMETER_MAX)
		{
			aniId = ID_ANI_MARIO_RACOON_JUMP_RUN;
		}
		else
		{
			if (vy > 0) aniId = ID_ANI_MARIO_RACOON_FALLING;
			else aniId = ID_ANI_MARIO_RACOON_JUMP_WALK;
		}
	}
	else
		if (isSitting)
		{
			if (!isOnSlope)
				aniId = ID_ANI_MARIO_RACOON_SIT;
			else
				aniId = ID_ANI_MARIO_RACOON_SLIDING;
		}
		else
			if (vx == 0)
			{
				aniId = ID_ANI_MARIO_RACOON_IDLE;
			}
			else
			{
				if (accelX == 0) aniId = ID_ANI_MARIO_RACOON_WALKING;
				else if (accelX * vx < 0 && accelX != 0) aniId = ID_ANI_MARIO_RACOON_SKIDDING;
				else if (abs(accelX) == MARIO_ACCEL_RUN_X)
				{
					if (pmeter == MARIO_PMETER_MAX) aniId = ID_ANI_MARIO_RACOON_RUNNING;
					else aniId = ID_ANI_MARIO_RACOON_WALKING;
				}
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
	else if (isPoofTransforming)
	{
		aniId = ID_ANI_MARIO_POOF;
		renderY = y;
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
		{
			aniId = GetAniIdRacoon();
			renderX -= 3.0f * nx;
		}
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

	/// ---------- Điều chỉnh pivot
	if (aniId == ID_ANI_MARIO_RACOON_SKIDDING)
	{
		renderY -= 2.0f;
	}
	if (aniId == ID_ANI_MARIO_RACOON_SPIN)
	{
		ULONGLONG time_passed = GetTickCount64() - spin_start;
		if (time_passed < (MARIO_SPIN_TIME) * 0.25)
		{
			renderX -= 1.0f * nx;
		}
		else if (time_passed >= (MARIO_SPIN_TIME) * 0.25 && time_passed < (MARIO_SPIN_TIME * 0.5))
		{
			renderX += 3.0f * nx;
		}
		else if (time_passed >= (MARIO_SPIN_TIME) * 0.5 && time_passed < (MARIO_SPIN_TIME) * 0.75)
		{
			renderX += 6.0f * nx;
		}
		else if (time_passed >= (MARIO_SPIN_TIME) * 0.75 && time_passed < (MARIO_SPIN_TIME) * 1.0)
		{
			renderX += 3.0f * nx;
		}
		else
		{
			renderX -= 1.0f * nx;
		}
	}

	if (shouldRender == true)
	{
		animations->Get(aniId)->Render(renderX, renderY, isFlip);
	}

	//RenderBoundingBox();
	
	DebugOutTitle(L"Pmeter: %d", pmeter);
}

void Mario::SetState(MarioState state)
{
	// DIE is the end state, cannot be changed! 
	if (this->state == static_cast<int>(MarioState::DIE)) return;

	if (isTakingDamage || isSuperTransforming || isPoofTransforming) return;

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
			if (pmeter == MARIO_PMETER_MAX)
			{
				if (form == MarioForm::SMALL)
				{
					vy = -MARIO_SMALL_JUMP_RUN_SPEED_Y;
				}
				else
				{
					vy = -MARIO_JUMP_RUN_SPEED_Y;
					vx *= 1.2f;
				}

				// nếu là gấu mèo thì bật bay
				if (form == MarioForm::RACOON)
				{
					canFly = true;
					fly_start = GetTickCount64();
				}
			}
			else // Nhảy bình thường khi chưa đầy P-Meter
			{
				vy = -MARIO_JUMP_SPEED_Y;
			}
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
			isSitting = true;
			y += MARIO_SIT_HEIGHT_ADJUST;
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
		if (isSitting) break;
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

void Mario::SetDirection(int d)
{
	if (isTakingDamage || isSuperTransforming || isSuperTransforming) return;
	nx = d;
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
	else if (this->form != MarioForm::SMALL && newForm == MarioForm::SMALL)
	{
		y += heightDiff;
	}

	this->form = newForm;
}


// ============================== TRANSFORM ===============================
void Mario::StartTransform()
{
	isSuperTransforming = true;
	transform_start = GetTickCount64();
	vx = vy = 0;
	accelX = accelY = 0;
}

void Mario::StartPoofTransform(MarioForm targetForm)
{
	isPoofTransforming = true;
	poof_start = GetTickCount64();
	nextPoofForm = targetForm;

	vx = vy = 0;
	accelX = accelY = 0;
}

// ============================== BEHAVIOUR ===============================
void Mario::Attack()
{
	if (form != MarioForm::RACOON || isSpinning == true) return;
	isSpinning = true;
	spin_start = GetTickCount64();
}


void Mario::TakeDamage()
{
	if (untouchable != 0) return;

	if (form == MarioForm::RACOON)
	{
		StartPoofTransform(MarioForm::SUPER);
		StartUntouchable();
	}
	else if (form > MarioForm::SMALL)
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

void Mario::Reset()
{
	//về vị trí cũ
	x = start_x;
	y = start_y;

	// Reset hình dáng
	SetNewForm(MarioForm::SMALL);
	SetState(MarioState::IDLE);

	vx = 0;
	vy = 0;
	accelX = 0;
	accelY = MARIO_GRAVITY;

	isTakingDamage = false;
	isSuperTransforming = false;
	isPoofTransforming = false;
	isSpinning = false;
	canFly = false;
	isFlying = false;
	isFloating = false;

	untouchable = 0;
	pmeter = 0;

	SetDirection(1);
}

// ============================== HANDLE UPDATE ===============================
#pragma region HANDLE UPDATE
void Mario::HandleSpinning(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (isSitting) return;
	if (isSpinning)
	{
		if (GetTickCount64() - spin_start > MARIO_SPIN_TIME)
			isSpinning = false;
		else
		{
			if ((GetTickCount64() - spin_start >= MARIO_SPIN_TIME * 0.25) && (GetTickCount64() - spin_start <= MARIO_SPIN_TIME * 0.75))
			{
				float ml, mt, mr, mb;
				GetBoundingBox(ml, mt, mr, mb);
				ml -= 12.0f;
				mr += 12.0f;
				for (size_t i = 0; i < coObjects->size(); i++)
				{
					LPGAMEOBJECT obj = coObjects->at(i);
					if (dynamic_cast<Goomba*>(obj))
					{
						float gl, gt, gr, gb;
						obj->GetBoundingBox(gl, gt, gr, gb);

						if (mr >= gl && ml <= gr && mb >= gt && mt <= gb)
						{
							Goomba* goomba = dynamic_cast<Goomba*>(obj);
							if (goomba->GetState() != GOOMBA_STATE_DIE)
							{
								// Goomba bị quất đuôi sẽ văng ngược lên trời
								goomba->SetState(GOOMBA_STATE_DIE);
								//goomba->SetVy(-0.2f); // nhay len
							}
						}
					}

					if (dynamic_cast<QuestionBlock*>(obj))
					{
						QuestionBlock* qb = dynamic_cast<QuestionBlock*>(obj);
						if (qb->GetState() == static_cast<int>(QuestionBlockState::ACTIVE))
						{
							float qbl, qbt, qbr, qbb;
							obj->GetBoundingBox(qbl, qbt, qbr, qbb);
							if (mr >= qbl && ml <= qbr && mb >= qbt && mt <= qbb)
							{
								qb->SetState(QuestionBlockState::BOUNCING);
							}
						}
					}


					if (dynamic_cast<Brick*>(obj))
					{
						Brick* brick = dynamic_cast<Brick*>(obj);
						if (brick->GetState() == static_cast<int>(BrickState::ACTIVE))
						{
							float qbl, qbt, qbr, qbb;
							obj->GetBoundingBox(qbl, qbt, qbr, qbb);
							if (mr >= qbl && ml <= qbr && mb >= qbt && mt <= qbb)
							{
								brick->Break();
							}
						}
					}
				}
			}
		}
	}
}

void Mario::HandleDying(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
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
}

void Mario::HandleTakingDamage(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (isTakingDamage)
	{
		if (GetTickCount64() - damage_start > MARIO_HIT_TIMEOUT)
		{
			isTakingDamage = false;
			if (form != MarioForm::SMALL && form != MarioForm::SUPER)
				SetNewForm(MarioForm::SUPER);
			else if (form == MarioForm::SUPER)
				SetNewForm(MarioForm::SMALL);
			accelY = MARIO_GRAVITY;
		}
		return;
	}
}

void Mario::HandleTransform(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
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

	if (isPoofTransforming)
	{
		if (GetTickCount64() - poof_start > MARIO_POOF_TIME)
		{
			isPoofTransforming = false;
			SetNewForm(nextPoofForm);
			accelY = MARIO_GRAVITY;
		}
		return;
	}
}

void Mario::HandlePMeter(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (isOnPlatform && abs(vx) >= MARIO_RUNNING_SPEED * 0.8f && state == static_cast<int>(MarioState::RUNNING))
	{
		if (GetTickCount64() - pmeter_start > MARIO_PMETER_CHARGE_TIME)
		{
			if (pmeter < MARIO_PMETER_MAX) pmeter++;
			pmeter_start = GetTickCount64();
		}
	}
	else if (isOnPlatform && pmeter > 0 && !isFlying && !canFly)
	{
		if (GetTickCount64() - pmeter_start > MARIO_PMETER_CHARGE_TIME * 1.5f)
		{
			pmeter--;
			pmeter_start = GetTickCount64();
		}
	}
}

void Mario::HandleSlope(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	float l, t, r, b;
	GetBoundingBox(l, t, r, b);
	float marioBottomY = b;
	float bboxHeight = b - t;
	//float marioBottomY = y + MARIO_BIG_BBOX_HEIGHT / 2; // tọa độ chân

	bool foundSlope = false;

	for (size_t i = 0; i < coObjects->size(); i++)
	{
		LPGAMEOBJECT obj = coObjects->at(i);
		if (dynamic_cast<Slope*>(obj))
		{
			Slope* slope = dynamic_cast<Slope*>(obj);
			//slopeDirection = slope->GetDirection();
			float sl, st, sr, sb;
			slope->GetBoundingBox(sl, st, sr, sb);

			// Rút tâm X của Mario ra
			float marioCenterX = x;

			// kiểm tra mario đang đứng trên dốc
			if (marioCenterX >= sl && marioCenterX <= sr && marioBottomY >= st && marioBottomY <= sb)
			{
				// lấy tọa độ y
				float expectedY = slope->GetSurfaceY(marioCenterX);

				// kéo mairo lên nếu chân < dốc
				if (marioBottomY >= expectedY - 4.0f && vy >=0) // -2.0f sai số nhỏ
				{
					y = expectedY - bboxHeight / 2;
					vy = 0;
					isOnPlatform = true;
					foundSlope = true;
					slopeDirection = slope->direction;
				}
			}
		}
	}

	isOnSlope = foundSlope;
}

void Mario::HandleSlopePhysics(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (isSitting && isOnPlatform)
	{
		if (isOnSlope)
		{
			int slideDirection = -slopeDirection;
			accelX = (MARIO_ACCEL_RUN_X * 1.3f) * slideDirection;
			maxVx = (MARIO_RUNNING_SPEED * 1.3f) * slideDirection;
			nx = slideDirection;
		}
		else
		{
			accelX = 0.0f;
		}
	}
}



/// Note tính toán phép xử lý lên/xuống dốc
/// Dốc đi lên có direction của slope là 1, mario đi sang phải thì chậm, đi sang trái thì nhanh
/// Dốc đi xuống có direction của slope là -1, mario đi sang phải thì nhanh còn đi sang trái thì chậm
/// ===> marioVx * slopeDirection > 0 thì sẽ đi chậm vì mario đi cùng chiều lên dốc
/// marioVx * slopeDirection < 0 thì đi nhanh hơn vì đây là đi xuống dốc
/// 


#pragma endregion