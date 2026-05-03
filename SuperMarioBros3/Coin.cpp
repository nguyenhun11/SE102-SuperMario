#include "Coin.h"
#include "PlayScene.h"
#include "Mario.h"
#include "ScoreEffect.h"

void Coin::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (state == static_cast<int>(CoinState::IDLE)) return;

	vy += COIN_GRAVITY * dt;
	y += vy * dt;
	if (y >= startY && vy > 0)
	{
		PlayScene* scene = dynamic_cast<PlayScene*>(SceneManager::GetInstance()->GetCurrentScene());
		Mario* mario = dynamic_cast<Mario*>(scene->GetPlayer());
		mario->AddScore();
		ScoreEffect* scoreEffect = new ScoreEffect(this->x, this->y, Score::ONE_HUNDRED);
		scene->AddObject(scoreEffect);
		this->Delete();

		// gọi hiệu ứng chớp chớp cái điểm số
		///
		///
	}
}

void Coin::Render()
{
	int aniId = GetCoinAnimID(state);
	Animations* animations = Animations::GetInstance();
	animations->Get(aniId)->Render(x, y);

	//RenderBoundingBox();
}

int Coin::GetCoinAnimID(int state)
{
	int aniID = -1;
	switch (state)
	{
	case static_cast<int>(CoinState::IDLE):
		aniID = ID_ANIM_COIN_IDLE;
		break;
	case static_cast<int>(CoinState::BOUNCING):
		aniID = ID_ANIM_COIN_BOUNCING;
		break;
	}
	return aniID;
}

void Coin::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - COIN_BBOX_WIDTH / 2;
	t = y - COIN_BBOX_HEIGHT / 2;
	r = l + COIN_BBOX_WIDTH;
	b = t + COIN_BBOX_HEIGHT;
}

void Coin::SetState(CoinState newState)
{
	switch (newState)
	{
	case CoinState::IDLE:
		vx = vy = 0;
		break;
	case CoinState::BOUNCING:
		vy -= COIN_BOUNCE_SPEED;
		break;
	}

	GameObject::SetState(static_cast<int>(newState));
}