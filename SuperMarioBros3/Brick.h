#pragma once

#include "GameObject.h"
#include "Animation.h"
#include "Animations.h"
#include "PlayScene.h"
#include "BrickDebris.h"
#include "Item.h"

#define ID_ANI_BRICK_ACTIVE 30100
#define ID_ANI_BRICK_EMPTY	30110

// boundary box
#define BRICK_BBOX_WIDTH 16
#define BRICK_BBOX_HEIGHT 16

// bounce config
#define BRICK_BOUNCE_SPEED 0.2f
#define BRICK_BOUNCE_HEIGHT 10.0f

enum class BrickItem {
	NONE = 0,
	P_SWITCH = 1,
	MUSHROOM = 2,
	ONE_UP_MUSHROOM = 3,
	COIN = 4,
};

enum class BrickState {
	ACTIVE = 0,
	BOUNCING = 1,
	BROKEN = 2,
	EMPTY = 3
};

class Brick : public GameObject {
protected:
	float startY;
	BrickState currentState;
	BrickItem containedItem;
	int containedSwitchType;

public:
	Brick(float x, float y, int itemType = 0, int switchType = 0);

	virtual void Render();
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);

	virtual void SetState(BrickState state);
	BrickState GetCurrentState() { return currentState; }
	BrickItem GetContainedItem() { return containedItem; }

	void Break();
	void SpawnItem();

	virtual int IsCollidable() { return (currentState != BrickState::BROKEN); }
	virtual int IsBlocking() { return (currentState != BrickState::BROKEN); }
};