#pragma once
#include "GameObject.h"

#define ID_SPR_WOODBLOCK_ACTIVE   30001
#define ID_SPR_WOODBLOCK_EMPTY    30001 // Có ảnh trống nhma xấu

#define WOODBLOCK_BBOX_WIDTH      16
#define WOODBLOCK_BBOX_HEIGHT     16
#define WOODBLOCK_BOUNCE_WIDTH    8.0f // Biên độ nảy ngang

#define WOODBLOCK_BOUNCE_SPEED    0.15f
#define WOODBLOCK_BOUNCE_HEIGHT   8.0f

enum class WoodBlockItem {
	NONE = 0,
	POWER_UP = 1,
	ONE_UP_MUSHROOM = 2
};

enum class WoodBlockState {
	ACTIVE = 0,
	BOUNCING = 1,
	EMPTY = 2
};

class WoodBlock : public GameObject {
protected:
	float startX;
	float startY;
	bool isBouncingX;
	bool isReturningX;
	bool isEmptied;

	WoodBlockState currentState;
	WoodBlockItem containedItem;

public:
	WoodBlock(float x, float y, int itemType = 0);

	virtual void Render();
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);

	virtual void SetState(WoodBlockState state);
	WoodBlockState GetCurrentState() { return currentState; }

	void SpawnItem();

	virtual int IsCollidable() { return 1; }
	virtual int IsBlocking() { return 1; }

	void HitHorizontally(float hit_nx);
	void HitVertically();
};

class WoodBlockSensor : public GameObject // Cảm biến trượt dọc
{
	WoodBlock* parent;
	float pushDirection;

public:
	WoodBlockSensor(float x, float y, WoodBlock* parent, float dir) : GameObject(x, y)
	{
		this->parent = parent;
		this->pushDirection = dir;
	}

	virtual void Render() {}
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {}

	virtual void GetBoundingBox(float& l, float& t, float& r, float& b)
	{
		l = x - 2.0f / 2;
		t = y - 16.0f / 2;
		r = l + 2.0f;
		b = t + 16.0f;
	}

	virtual int IsCollidable() { return 1; }
	virtual int IsBlocking() { return 0; }

	WoodBlock* GetParent() { return parent; }
	float GetDirection() { return pushDirection; }
};