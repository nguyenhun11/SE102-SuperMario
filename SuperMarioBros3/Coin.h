#pragma once

#include "GameObject.h"
#include "Animation.h"
#include "Animations.h"


#define	COIN_WIDTH 10
#define COIN_BBOX_WIDTH 16
#define COIN_BBOX_HEIGHT 16

// animation
#define ID_ANIM_COIN_IDLE	4010
#define ID_ANIM_COIN_BOUNCING	4011

enum class CoinState
{
	IDLE = 0,	
	BOUNCING = 1
};

#define COIN_BOUNCE_SPEED	0.4f
#define	COIN_GRAVITY	0.0015f

class Coin : public GameObject {
protected:
	float startY;

public:
	Coin(float x, float y) : GameObject(x, y)
	{
		this->startY = y;
		this->state = static_cast<int>(CoinState::IDLE);
		this->vy = 0;

		zIndex = 4;
	}
	void Render();
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	void SetState(CoinState state);

	int IsCollidable()
	{
		return (state == static_cast<int>(CoinState::IDLE));
	}
	int IsBlocking() { return 0; }


	int GetCoinAnimID(int state);
};