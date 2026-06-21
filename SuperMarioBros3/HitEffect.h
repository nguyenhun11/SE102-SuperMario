#include "GameObject.h"

#define ID_ANI_HIT_EFFECT 5040


#pragma once
class HitEffect : public GameObject
{
protected:
	ULONGLONG effect_start;
public:
	HitEffect(float x, float y) : GameObject(x, y)
	{
		effect_start = GetTickCount64();
		vy = 0;
		vx = 0;
		zIndex = 14;
	}
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void Render();

	void GetBoundingBox(float& left, float& top, float& right, float& bottom);
};

