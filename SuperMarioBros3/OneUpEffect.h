#include "GameObject.h"

#define ID_ANI_ONE_UP_EFFECT 5020


#pragma once
class OneUpEffect : public GameObject
{
protected:
	ULONGLONG effect_start;
public:
	OneUpEffect(float x, float y);
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void Render();

	void GetBoundingBox(float& left, float& top, float& right, float& bottom);
};

