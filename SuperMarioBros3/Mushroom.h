#pragma once
#include "GameObject.h"

// vạt ly
#define MUSHROOM_GRAVITY 0.002f
#define MUSHROOM_WALKING_SPEED 0.05f
#define MUSHROOM_SPAWN_SPEED 0.02f 

#define MUSHROOM_BBOX_WIDTH 16
#define MUSHROOM_BBOX_HEIGHT 16

// anim
#define ID_ANIM_MUSHROOM 4020

enum class MushroomState {
	SPAWNING = 0, // trồi lên
	WALKING = 1   // Đã trồi xong, bắt đầu rớt và đi
};

class Mushroom : public GameObject
{
protected:
	float startY;

public:
	Mushroom(float x, float y);

	virtual void Render();
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);

	virtual void SetState(MushroomState state);

	virtual void OnNoCollision(DWORD dt);
	virtual void OnCollisionWith(LPCOLLISIONEVENT e);

	virtual int IsCollidable()
	{
		return (state != static_cast<int>(MushroomState::SPAWNING));
	}

	virtual int IsBlocking() { return 0; }
};