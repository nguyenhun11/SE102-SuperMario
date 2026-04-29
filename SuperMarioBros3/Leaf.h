#pragma once
#include "GameObject.h"

#define LEAF_BBOX_WIDTH 16
#define LEAF_BBOX_HEIGHT 14

#define LEAF_SPAWN_SPEED_Y 0.12f
#define LEAF_GRAVITY 0.0003f
#define LEAF_FALLING_SPEED_Y 0.03f
#define LEAF_HORIZONTAL_SPEED 0.065f
#define LEAF_AMPLITUDE 40.0f // độ rộng zigzag

#define ID_ANIM_LEAF 4030

enum class LeafState {
	SPAWNING = 0, 
	FALLING = 1  
};

class Leaf : public GameObject
{
	float startX;
	float startY;

public:
	Leaf(float x, float y);
	virtual void Render();
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);

	virtual void SetState(LeafState state);
	virtual int IsCollidable() { return 1; }
	virtual int IsBlocking() { return 0; }
};