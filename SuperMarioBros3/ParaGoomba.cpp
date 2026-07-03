#include "ParaGoomba.h"
#include "PlayScene.h"
#include "SceneManager.h"
#include "Mario.h"

ParaGoomba::ParaGoomba(float x, float y)
	: Goomba(x, y)
{
	hasWings = true;
	isAirborne = false;
	justLanded = false;

	flightPhase = FlightPhase::WALK;
	phaseStart = GetTickCount64();
	hopDoneCount = 0;

	OnEnable();
}

void ParaGoomba::OnEnable()
{
	Goomba::OnEnable();

	hasWings = true;
	isAirborne = false;
	justLanded = false;
	isPausing = false;

	flightPhase = FlightPhase::WALK;
	phaseStart = GetTickCount64();
	hopDoneCount = 0;
}

void ParaGoomba::SetState(GoombaState state)
{
	if (state == GoombaState::DIE && hasWings)
	{
		hasWings = false;
		flightPhase = FlightPhase::WALK;
		hopDoneCount = 0;
		phaseStart = GetTickCount64();

		Goomba::SetState(GoombaState::WALKING);
		return;
	}

	Goomba::SetState(state);

	if (state == GoombaState::BOUNCE)
	{
		hasWings = false;
	}
}

void ParaGoomba::OnStompedByMario()
{
	if (hasWings)
	{
		hasWings = false;
		flightPhase = FlightPhase::WALK;
		hopDoneCount = 0;
		phaseStart = GetTickCount64();

		Goomba::SetState(GoombaState::WALKING);
	}
	else
	{
		Goomba::SetState(GoombaState::DIE);
	}
}

void ParaGoomba::OnHitSideways()
{
	Goomba::SetState(GoombaState::BOUNCE);
}

void ParaGoomba::StartPhase(FlightPhase phase)
{
	flightPhase = phase;
	phaseStart = GetTickCount64();

	if (phase == FlightPhase::WALK)
	{
		hopDoneCount = 0;
		FaceTowardPlayer();
	}
}

void ParaGoomba::FaceTowardPlayer()
{
	PlayScene* scene = (PlayScene*)SceneManager::GetInstance()->GetCurrentScene();
	if (scene == nullptr) return;

	Mario* mario = (Mario*)scene->GetPlayer();
	if (mario == nullptr) return;

	nx = (mario->GetX() > x) ? 1 : -1;
	vx = nx * PARAGOOMBA_WALKING_SPEED;
}

void ParaGoomba::UpdateFlightPattern(DWORD dt)
{
	if (!hasWings) return;

	switch (flightPhase)
	{
	case FlightPhase::WALK:
		vx = nx * PARAGOOMBA_WALKING_SPEED;

		if (GetTickCount64() - phaseStart >= PARAGOOMBA_WALK_DURATION)
		{
			StartPhase(FlightPhase::HOP);
			isAirborne = true;
			vy = PARAGOOMBA_HOP_VY;
		}
		break;

	case FlightPhase::HOP:
		// Đang nghỉ trên đất giữa 2 hop -> chờ hết giờ mới nhảy tiếp
		if (isPausing)
		{
			vx = 0; // đứng yên khi nghỉ, cho rõ nhịp
			if (GetTickCount64() - groundPauseStart < PARAGOOMBA_HOP_PAUSE) return;
			isPausing = false;

			if (hopDoneCount < PARAGOOMBA_HOP_COUNT)
			{
				isAirborne = true;
				vy = PARAGOOMBA_HOP_VY;
				vx = nx * PARAGOOMBA_WALKING_SPEED;
			}
			else
			{
				flightPhase = FlightPhase::JUMP;
				isAirborne = true;
				vy = PARAGOOMBA_JUMP_VY;
				vx = nx * PARAGOOMBA_WALKING_SPEED;
			}
			return;
		}

		if (!justLanded) return;
		justLanded = false;

		hopDoneCount++;

		// Vừa chạm đất -> KHÔNG bay ngay, chuyển sang trạng thái nghỉ
		isPausing = true;
		groundPauseStart = GetTickCount64();
		vx = 0;
		break;

	case FlightPhase::JUMP:
		if (isPausing)
		{
			vx = 0;
			if (GetTickCount64() - groundPauseStart < PARAGOOMBA_JUMP_PAUSE) return;
			isPausing = false;
			StartPhase(FlightPhase::WALK);
			return;
		}

		if (!justLanded) return;
		justLanded = false;

		// Vừa hạ cánh sau cú nhảy vọt -> nghỉ 1 chút trước khi quay lại đi bộ
		isPausing = true;
		groundPauseStart = GetTickCount64();
		vx = 0;
		break;
	}
}

void ParaGoomba::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	UpdateFlightPattern(dt);
	Goomba::Update(dt, coObjects);
}

void ParaGoomba::OnCollisionWith(LPCOLLISIONEVENT e)
{
	Goomba::OnCollisionWith(e);

	if (!hasWings) return;
	if (!e->obj->IsBlocking()) return;

	if (e->ny < 0) // chạm nền
	{
		if (isAirborne)
		{
			// Cạnh lên: vừa từ trên không -> chạm đất -> chỉ bắn 1 lần
			isAirborne = false;
			justLanded = true;
		}
		vy = 0;
	}

	if (e->nx != 0)
	{
		vx = nx * PARAGOOMBA_WALKING_SPEED;
	}
}

void ParaGoomba::Render()
{
	float renderX = x;
	float renderY = y;

	if (state == static_cast<int>(GoombaState::DIE))
	{
		Animations::GetInstance()->Get(ID_ANI_PARAGOOMBA_DIE)
			->Render(renderX, renderY - 2.0f, false, isFlippedVertical);
		return;
	}

	if (state == static_cast<int>(GoombaState::BOUNCE))
	{
		Animations::GetInstance()->Get(ID_ANI_PARAGOOMBA_BOUNCE)
			->Render(renderX, renderY, false, isFlippedVertical);
		return;
	}

	if (hasWings && nx < 0)
	{
		Animations::GetInstance()->Get(ID_ANI_WING_FLAP)
			->Render(renderX, renderY, true, false);
	}

	Animations::GetInstance()->Get(ID_ANI_PARAGOOMBA_WALKING)
		->Render(renderX, renderY, false, isFlippedVertical);

	if (hasWings && nx >= 0)
	{
		Animations::GetInstance()->Get(ID_ANI_WING_FLAP)
			->Render(renderX, renderY, false, false);
	}
}