#pragma once
#include "GameObject.h"
#include "Sprites.h"
#include "debug.h"

#define SOLID_BLOCK_BBOX_WIDTH 16
#define SOLID_BLOCK_BBOX_HEIGHT 16

class SolidBlock : public GameObject {
protected:
	int spriteId;

public:
	SolidBlock(float x, float y, int spriteId) : GameObject(x, y)
	{
		this->spriteId = spriteId;
		this->zIndex = 4;
	}

	virtual void Render() { Sprites::GetInstance()->Get(spriteId)->Draw(x, y); }

	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {}

	virtual void GetBoundingBox(float& l, float& t, float& r, float& b)
	{
		l = x - SOLID_BLOCK_BBOX_WIDTH / 2;
		t = y - SOLID_BLOCK_BBOX_HEIGHT / 2;
		r = l + SOLID_BLOCK_BBOX_WIDTH;
		b = t + SOLID_BLOCK_BBOX_HEIGHT;
	}

	virtual int IsCollidable() { return 1; }
	virtual int IsBlocking() { return 1; }
};