#pragma once
#include "GameObject.h"

#define DEBRIS_GRAVITY 0.001f
#define DEBRIS_BOUNCE_SPEED_X 0.08f
#define DEBRIS_BOUNCE_SPEED_Y_HIGH 0.4f // Dành cho 2 mảnh văng lên cao
#define DEBRIS_BOUNCE_SPEED_Y_LOW 0.2f  // Dành cho 2 mảnh văng thấp

#define ID_ANI_BRICK_DEBRIS 30120 // ID animation mảnh vỡ xoay vòng vòng

class BrickDebris : public GameObject
{
protected:
	float ax;
	float ay;

public:
	// Truyền hướng (vx) và độ cao (vy) ngay lúc khởi tạo
	BrickDebris(float x, float y, float vx, float vy);

	virtual void Render();
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b) {} // Xuyên thấu

	virtual int IsCollidable() { return 0; }
	virtual int IsBlocking() { return 0; }
};