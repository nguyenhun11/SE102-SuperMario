#pragma once
#include "GameObject.h"

#define BRICK_TO_COIN_OFF_ANI_ID 4050101
#define BRICK_TO_COIN_ON_ANI_ID 4050100
#define COIN_TO_BRICK_OFF_ANI_ID 4050201
#define COIN_TO_BRICK_ON_ANI_ID 4050200

enum class SwitchState
{
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
	SwitchState state;
	SwitchType type;

public:
	Switch(SwitchType type) : GameObject() { state = SwitchState::Off; this->type = type; }

	SwitchType GetType() { return type; }
	void SetType(SwitchType type) { this->type = type; }

	bool IsPressed() { return state == SwitchState::On; }
	void SetPressed() { this->state = SwitchState::On; }

	void Render() override;
	void GetBoundingBox(float& left, float& top, float& right, float& bottom) override;
	
	int IsBlocking() override { return state == SwitchState::Off; }
	int IsCollidable() override { return state == SwitchState::Off; }
};

