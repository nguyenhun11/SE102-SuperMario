#pragma once
#include "GameObject.h"

#define GOALBLOCK_BBOX_WIDTH 16
#define GOALBLOCK_BBOX_HEIGHT 16
#define GOALBLOCK_ROULETTE_CHANGE_TIME 150
#define GOALBLOCK_CARD_MUSHROOM 3090
#define GOALBLOCK_CARD_FLOWER 3091
#define GOALBLOCK_CARD_STAR 3092
#define GOALBLOCK_ANI_MUSHROOM_FLY 3096
#define GOALBLOCK_ANI_FLOWER_FLY 3097
#define GOALBLOCK_ANI_STAR_FLY 3098

// trạng thái
enum class GoalBlockState 
{ 
    ROULETTE = 1, 
    FINISHED = 2
};

// loại thẻ sẽ sinh ra
enum class CardType 
{ 
    MUSHROOM = 0, 
    FLOWER = 1, 
    STAR = 2
};

class GoalBlock : public GameObject 
{
private:
    GoalBlockState state;
    CardType card = CardType::MUSHROOM;
    ULONGLONG last_change = 0;
    bool isFinished = false;

public:
    GoalBlock(float x, float y) : GameObject(x, y)
    {
        state = GoalBlockState::ROULETTE;
    }

    void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);

    void SetFinished() 
    {
        state = GoalBlockState::FINISHED;
        vy = -0.08f;
    }

    void Render();
    int IsCollidable()
    {
        return (state != GoalBlockState::FINISHED);
    }
    int IsBlocking() { return 0; };
    void GetBoundingBox(float& l, float& t, float& r, float& b);

    CardType GetCardType() { return card; }
	GoalBlockState GetBlockState() { return state; }
};