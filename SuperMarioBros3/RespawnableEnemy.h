#pragma once
#include "GameObject.h"

class RespawnPoint;
class RespawnableEnemy : public GameObject
{
	RespawnPoint* respawnPoint = nullptr;
	int flyDirection = 1;
protected:
	float ax;
	float ay;
	bool IsOtherEnemyOrMario(LPGAMEOBJECT obj);
public:
	RespawnableEnemy(float x, float y);
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) override;
	void OnExitCamera() override;
	void Delete() override;
	virtual void OnAttackedByTail(float direction);
	virtual void SetState(int state) override;
	bool RenderOnPaused() override { return false; }
};

class RespawnPoint : public GameObject {
private:
	RespawnableEnemy* obj = nullptr;
public:
	RespawnPoint(float x, float y, RespawnableEnemy* obj);
	void Render() override {}
	void GetBoundingBox(float& l, float& t, float& r, float& b) override {}
	void OnEnterCamera() override;
	void Delete() override;
};

