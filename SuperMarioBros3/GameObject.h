#pragma once

#include <Windows.h>
#include <d3dx10.h>
#include <vector>

#include "Animation.h"
#include "Animations.h"
#include "Sprites.h"
#include "Collision.h"

using namespace std;

#define ID_TEX_BBOX -100		// special texture to draw object bounding box
#define BBOX_ALPHA 0.25f		// Bounding box transparency

class GameObject
{
protected:

	float x;
	float y;

	float vx;
	float vy;

	int nx;

	int state;

	bool isDeleted;

	bool isActive;
	bool wasOnCamera;

	int zIndex;


public:
	bool isCreatedBySwitch = false; // để phân biệt với brick được tạo ra từ switch

	void SetPosition(float x, float y) { this->x = x; this->y = y; }
	void SetSpeed(float vx, float vy) { this->vx = vx; this->vy = vy; }
	void GetPosition(float& x, float& y) { x = this->x; y = this->y; }
	void GetSpeed(float& vx, float& vy) { vx = this->vx; vy = this->vy; }

	int GetState() { return this->state; }
	virtual void Delete() { isDeleted = true; }
	bool IsDeleted() { return isDeleted; }

	virtual void OnEnable() {}
	virtual void OnDisable() {}

	virtual void SetActive(bool active) {
		this->isActive = active;
		if (active) OnEnable();
		else OnDisable();
	}
	bool GetActive() { return this->isActive; }

	virtual void OnEnterCamera() {}
	virtual void OnExitCamera() {}
	virtual bool IsOnCamera();
	void CheckCameraStatus();


	void RenderBoundingBox();

	virtual bool RenderOnPaused() { return true; }

	GameObject();
	GameObject(float x, float y) :GameObject() {
		this->x = x;
		this->y = y;
		zIndex = 5;
		isActive = true;
		wasOnCamera = false;
	}


	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom) = 0;
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = NULL) {};
	virtual void Render() = 0;
	virtual void SetState(int state) { this->state = state; }

	// Collision ON or OFF ? This can change depending on object's state. For example: die
	//
	virtual int IsCollidable() { return 0; };

	// When no collision has been detected (triggered by CCollision::Process)
	virtual void OnNoCollision(DWORD dt) {};

	// When collision with an object has been detected (triggered by CCollision::Process)
	virtual void OnCollisionWith(LPCOLLISIONEVENT e) {};

	// Is this object blocking other object? If YES, collision framework will automatically push the other object
	virtual int IsBlocking() { return 1; }

	// Does this object collide with other object at certain direction ( like ColorBox )
	virtual int IsDirectionColliable(float nx, float ny) { return 1; }

	~GameObject();

	static bool IsDeleted(const LPGAMEOBJECT& o) { return o->isDeleted; }

	int GetZIndex() { return zIndex; }
	float GetX() { return x; }
	float GetY() { return y; }

	void SetNx(int n) { nx = n; }
	float GetVy() { return vy; }

	bool GetWasOnCamera() { return wasOnCamera; }
	void SetWasOnCamera(bool status) { wasOnCamera = status; }

};
