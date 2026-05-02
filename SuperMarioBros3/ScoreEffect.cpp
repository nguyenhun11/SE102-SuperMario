#include "ScoreEffect.h"

ScoreEffect::ScoreEffect(float x, float y, Score score) : GameObject(x, y)
{
	effect_start = GetTickCount64();
	vy = -0.05f;
	vx = 0;
	this->score = score;
}

void ScoreEffect::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	y += vy * dt;

	if (GetTickCount64() - effect_start > 500)
	{
		this->Delete();
	}
}

void ScoreEffect::Render()
{
	if(score == Score::ONE_HUNDRED)
		Animations::GetInstance()->Get(ID_ANI_SCORE_100)->Render(x, y);
	else if(score == Score::TWO_HUNDRED)
		Animations::GetInstance()->Get(ID_ANI_SCORE_200)->Render(x, y);
	else if(score == Score::FOUR_HUNDRED)
		Animations::GetInstance()->Get(ID_ANI_SCORE_400)->Render(x, y);
	else if(score == Score::EIGHT_HUNDRED)
		Animations::GetInstance()->Get(ID_ANI_SCORE_800)->Render(x, y);
	else if(score == Score::ONE_THOUSAND)
		Animations::GetInstance()->Get(ID_ANI_SCORE_1000)->Render(x, y);
}