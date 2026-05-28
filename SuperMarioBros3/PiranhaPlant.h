#pragma once
#include "GameObject.h"

#define PIRANHA_ARISE_SPEED 0.05f
#define PIRANHA_WAIT_TIME 2000
#define PIRANHA_TOP_WAIT 500

#define PIRANHA_BBOX_WIDTH 15
#define PIRANHA_BBOX_HEIGHT 19
#define PIRANHA_HEIGHT 17

#define PIRANHA_FIRE_INTERVAL 2000	

// Animation IDs
// NOTE: Thêm 3 animation này vào file asset .txt của scene
#define ID_ANI_PIRANHA			14016	// hoa ngửa lên (loại thường)
#define ID_ANI_PIRANHA_LOOK_UP	14017	// hoa nhìn lên (loại bắn lửa, Mario ở cao hơn)
#define ID_ANI_PIRANHA_LOOK_DOWN 14018	// hoa nhìn xuống (loại bắn lửa, Mario ở thấp hơn)

class PiranhaPlant : public GameObject
{
protected:
	ULONGLONG wait_start = 0;
	ULONGLONG top_start = 0;
	ULONGLONG fire_timer = 0;		// timer để bắn lửa

	bool isWaiting = false;
	float originalY;
	bool isGoingUp = true;
	bool isAtTop = false;

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

	virtual int IsCollidable() { return 1; };
	virtual int IsBlocking() { return 0; }
	virtual void OnNoCollision(DWORD dt);
	virtual void OnCollisionWith(LPCOLLISIONEVENT e);

	void ShootFire(float marioX, float marioY);

public:
	PiranhaPlant(float x, float y) : GameObject(x, y) {
		originalY = y;
		nx = 1;
	};
	bool GetIsWaiting() { return y == originalY; };
};
