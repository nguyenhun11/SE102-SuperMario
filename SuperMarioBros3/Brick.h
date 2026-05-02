#pragma once

#include "GameObject.h"
#include "Animation.h"
#include "Animations.h"

#define ID_ANI_BRICK 30100

// boundary box
#define BRICK_BBOX_WIDTH 16
#define BRICK_BBOX_HEIGHT 16

// bounce config
#define BRICK_BOUNCE_SPEED 0.2f
#define BRICK_BOUNCE_HEIGHT 10.0f

enum class BrickState {
	ACTIVE = 0,
	BOUNCING = 1,
	BROKEN = 2
};

class Brick : public GameObject {
protected:
	float startY;
	BrickState currentState;

public:
	Brick(float x, float y);

	virtual void Render();
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);

	virtual void SetState(BrickState state);
	BrickState GetCurrentState() { return currentState; }

	void Break();

	virtual int IsCollidable() { return (currentState != BrickState::BROKEN); }
	virtual int IsBlocking() { return (currentState != BrickState::BROKEN); }
};