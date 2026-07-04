#include "Lift.h"

void Lift::OnEnterCamera()
{
	if (state == (int)LiftState::Idle) {
		SetState((int)LiftState::Moving);
	}
}

void Lift::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (state == (int)LiftState::Idle) return;

	GameObject::Update(dt);

	if (state == (int)LiftState::Moving) {
		x += vx * dt;
	}
	else if (state == (int)LiftState::Falling) {
		vy += LIFT_GRAVITY * dt;
		y += vy * dt;
	}
}

void Lift::SetState(int state)
{
	GameObject::SetState(state);
	switch ((LiftState)state)
	{
	case LiftState::Idle:
		vx = 0;
		vy = 0;
		break;
	case LiftState::Moving:
		vx = -LIFT_SPEED_X;
		vy = 0;
		break;
	case LiftState::Falling:
		vx = 0;
		vy = 0.0f;
		break;
	}
}
