#include "GoalBlock.h"

void GoalBlock::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) 
{
	if (state == GoalBlockState::ROULETTE) {
		if (GetTickCount64() - last_change > GOALBLOCK_ROULETTE_CHANGE_TIME) 
		{ 
			// random card
			if (card == CardType::MUSHROOM)
			{
				card = CardType::FLOWER;
			}
			else if (card == CardType::FLOWER)
			{
				card = CardType::STAR;
			}
			else if (card == CardType::STAR)
			{
				card = CardType::MUSHROOM;
			}
			last_change = GetTickCount64();
		}
	}
	else
	{
		y += vy * dt;
	}

	Collision::GetInstance()->Process(this, dt, coObjects);
}

void GoalBlock::GetBoundingBox(float& l, float& t, float& r, float& b) 
{
	l = x - GOALBLOCK_BBOX_WIDTH / 2;
	t = y - GOALBLOCK_BBOX_HEIGHT / 2;
	r = x + GOALBLOCK_BBOX_WIDTH / 2;
	b = y + GOALBLOCK_BBOX_HEIGHT / 2;
}

void GoalBlock::Render()
{
	int aniId = GOALBLOCK_CARD_MUSHROOM;
	if (state == GoalBlockState::ROULETTE)
	{
		if(card == CardType::MUSHROOM) aniId = GOALBLOCK_CARD_MUSHROOM;
		else if (card == CardType::FLOWER) aniId = GOALBLOCK_CARD_FLOWER;
		else if (card == CardType::STAR) aniId = GOALBLOCK_CARD_STAR;
	}
	else
	{
		if (card == CardType::MUSHROOM) aniId = GOALBLOCK_ANI_MUSHROOM_FLY;
		else if (card == CardType::FLOWER) aniId = GOALBLOCK_ANI_FLOWER_FLY;
		else if (card == CardType::STAR) aniId = GOALBLOCK_ANI_STAR_FLY;
	}

	Animations::GetInstance()->Get(aniId)->Render(x, y);
}