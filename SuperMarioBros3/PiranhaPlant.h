#pragma once
#include "GameObject.h"

#define PIRANHA_TYPE_RED_FIRE       10  // Hoa đỏ bắn lửa
#define PIRANHA_TYPE_GREEN_NORMAL   20  // Hoa xanh cắn (Không lửa)
#define PIRANHA_TYPE_GREEN_FIRE     30  // Hoa xanh bắn lửa

#define PIRANHA_ARISE_SPEED 0.05f
#define PIRANHA_WAIT_TIME 2000
#define PIRANHA_TOP_WAIT 1000

#define PIRANHA_BBOX_WIDTH 16.0f
#define PIRANHA_BBOX_HEIGHT 32.0f
#define PIRANHA_HEIGHT 32.0f

#define PIRANHA_FIRE_INTERVAL 2000	

// HOA ĐỎ LỬA
#define ID_ANI_PIRANHA_RED_FIRE_DOWN    14016
#define ID_ANI_PIRANHA_RED_FIRE_UP      14017

// HOA XANH THƯỜNG
#define ID_ANI_PIRANHA_GREEN_NORMAL     14020

// HOA XANH LỬA
#define ID_ANI_PIRANHA_GREEN_FIRE_DOWN  14036 
#define ID_ANI_PIRANHA_GREEN_FIRE_UP    14037

class PiranhaPlant : public GameObject
{
protected:
	int plantType;
	float ariseHeight;

	ULONGLONG wait_start = 0;
	ULONGLONG top_start = 0;
	ULONGLONG fire_timer = 0;		// timer để bắn lửa

	bool isWaiting = false;
	float originalY;
	bool isGoingUp = true;
	bool isAtTop = false;
	bool hasShot = false;

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

	virtual int IsCollidable() { return 1; };
	virtual int IsBlocking() { return 0; }

	void ShootFire(float marioX, float marioY);

public:
	PiranhaPlant(float x, float y,
		int type = PIRANHA_TYPE_RED_FIRE, float height = 32.0f) : GameObject(x, y), plantType(type), ariseHeight(height) {
		originalY = y;
		nx = 1;
	};
	bool GetIsWaiting() { return y == originalY; };
};
