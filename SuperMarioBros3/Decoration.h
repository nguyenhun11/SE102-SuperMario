#pragma once
#include "GameObject.h"
class Decoration : public GameObject
{
	int spriteId;
public:
	Decoration(float x, float y, int sprite_id)
	{
		this->x = x;
		this->y = y;
		this->spriteId = sprite_id;
		zIndex = 0;

		isHiddenOnPause = false;
	}
	int IsBlocking() override { return 0; }
	void GetBoundingBox(float& l, float& t, float& r, float& b) override
	{
		l = 0; t = 0; r = 0; b = 0;
	}
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) override {}
	void Render() override
	{
		Sprites::GetInstance()->Get(spriteId)->DrawOnCamera(x, y);
	}
};

