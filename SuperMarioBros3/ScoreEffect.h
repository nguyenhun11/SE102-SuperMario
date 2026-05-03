#include "GameObject.h"

#define ID_ANI_SCORE_100 5010
#define ID_ANI_SCORE_200 5011
#define ID_ANI_SCORE_400 5012
#define ID_ANI_SCORE_800 5013
#define ID_ANI_SCORE_1000 5014

enum class Score
{
	ONE_HUNDRED,
	TWO_HUNDRED,
	FOUR_HUNDRED,
	EIGHT_HUNDRED,
	ONE_THOUSAND
};

#pragma once
class ScoreEffect : public GameObject
{
	ULONGLONG effect_start;
	Score score;
public:
	ScoreEffect(float x, float y, Score score);
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void Render();
	void GetBoundingBox(float& left, float& top, float& right, float& bottom) { left = top = right = bottom = 0; };
};

