#pragma once

#include "GameObject.h"
#include "Animation.h"
#include "Animations.h"

#define ID_ANI_FIRE 14100

#define FIRE_WIDTH 7
#define FIRE_BBOX_WIDTH 7
#define FIRE_BBOX_HEIGHT 7

#define FIRE_SPEED 0.08f

// Kích thước màn hình (dùng để check out-of-bounds)
#define SCREEN_WIDTH  270
#define SCREEN_HEIGHT 250

class Fire : public GameObject {
public:
	Fire(float x, float y) : GameObject(x, y) {};

	void Render();
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void GetBoundingBox(float& l, float& t, float& r, float& b);

	virtual int IsCollidable() { return 1; }
	virtual int IsBlocking() { return 0; }		
	virtual void OnCollisionWith(LPCOLLISIONEVENT e);

	void SetSpeed(float vx, float vy) { this->vx = vx; this->vy = vy; }
};
