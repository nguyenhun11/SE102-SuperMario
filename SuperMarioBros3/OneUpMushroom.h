#pragma once
#include "Core.h"

// anim
#define ID_ANIM_ONE_UP_MUSHROOM 4040

class OneUpMushroom : public Mushroom
{
protected:
	float dir; // 1 đi phải, -1 đi trái
public:
	OneUpMushroom(float x, float y, float dir = 1) : Mushroom(x, y) {
		this->dir = dir;
		isHiddenOnPause = true;
	}
	void SetState(MushroomState newState);
	void OnCollisionWith(LPCOLLISIONEVENT e);
	void Render();
	float GetX() { return x; }
	float GetY() { return y; }
};