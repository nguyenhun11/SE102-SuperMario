#pragma once
#include "GameObject.h"

#define BRICK_TO_COIN_OFF_ANI_ID 4050101
#define BRICK_TO_COIN_ON_ANI_ID 4050100
#define COIN_TO_BRICK_OFF_ANI_ID 4050201
#define COIN_TO_BRICK_ON_ANI_ID 4050200

#define SWITCH_SPAWN_SPEED 0.05f // Vận tốc trồi lên, bạn có thể chỉnh cho mượt

enum class SwitchState
{
	Spawning,
	Off,
	On
};

enum class SwitchType
{
	BrickToCoin,
	CoinToBrick
};

class Switch : public GameObject
{
private:
	SwitchType type;
	float startY; // Dùng để đo quãng đường trồi lên

public:
	Switch(float x, float y, SwitchType type);

	SwitchType GetType() { return type; }
	void SetType(SwitchType type) { this->type = type; }

	bool IsPressed() { return this->state == (int)SwitchState::On; }
	void SetPressed() { SetState((int)SwitchState::On); }

	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) override;
	void Render() override;
	void GetBoundingBox(float& left, float& top, float& right, float& bottom) override;
	void SetState(int state) override;

	int IsBlocking() override { return this->state == (int)SwitchState::Off; }
	int IsCollidable() override { return this->state == (int)SwitchState::Off; }
};