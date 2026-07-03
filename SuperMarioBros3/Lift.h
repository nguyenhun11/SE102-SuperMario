#pragma once
#include "NineSliceBox.h"

#define LIFT_SPEED_X 0.015f
#define LIFT_SPEED_FALL 0.05f  

enum class LiftState
{
	Idle = 0,
	Moving,
	Falling
};


class Lift : public NineSliceBox
{
public:
	Lift(float x, float y, float cell_width, float cell_height, int length,
		int idLeft, int idMid, int idRight
	)
		: NineSliceBox(x, y, cell_width, cell_height, length, 1, nullptr, true, false, true, true)
	{
		this->spriteIDs[0][0] = idLeft;
		this->spriteIDs[0][1] = idMid;
		this->spriteIDs[0][2] = idRight;

		SetState((int)LiftState::Idle);
	}

	void OnEnterCamera() override;
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) override;
	void SetState(int state) override;

	int IsCollidable() override { return 1; }
	int IsBlocking() override { return 1; }
	int IsDirectionColliable(float nx, float ny) override
	{
		if (nx != 0.0f) return 0;
		if (ny > 0.0f) return 0;
		return 1;
	}
};

