#include "QuestionBlock.h"
#include "debug.h"

QuestionBlock::QuestionBlock(float x, float y, CONTAINED_OBJECT containedItem) : GameObject(x, y)
{
	this->startY = y;
	this->item = containedItem;
	this->currentState = QuestionBlockState::ACTIVE;
	this->state = static_cast<int>(QuestionBlockState::ACTIVE);
	this->vy = 0;
}

void QuestionBlock::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - QUESTION_BLOCK_BBOX_WIDTH / 2;
	t = y - QUESTION_BLOCK_BBOX_HEIGHT / 2;
	r = l + QUESTION_BLOCK_BBOX_WIDTH;
	b = t + QUESTION_BLOCK_BBOX_HEIGHT;
}

void QuestionBlock::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	y += vy * dt;

	if (currentState == QuestionBlockState::BOUNCING)
	{
		if (startY - y >= QBLOCK_BOUNCE_HEIGHT && vy < 0)
		{
			vy = QBLOCK_BOUNCE_SPEED;
		}
		
		else if (y >= startY && vy > 0)
		{
			y = startY;
			vy = 0;
			SetState(QuestionBlockState::EMPTY);

			// sinh ra item khối này đang đựng ở đây
			DebugOut(L"thả item!\n");
		}
	}
}

void QuestionBlock::Render()
{
	int aniId = ID_ANIM_QUESTION_BLOCK_ACTIVE; 

	if (currentState == QuestionBlockState::EMPTY || currentState == QuestionBlockState::BOUNCING)
	{
		aniId = ID_ANIM_QUESTION_BLOCK_EMPTY;
	}

	Animations::GetInstance()->Get(aniId)->Render(x, y);
}

void QuestionBlock::SetState(QuestionBlockState state)
{
	this->currentState = state;
	GameObject::SetState(static_cast<int>(state));

	switch (state)
	{
	case QuestionBlockState::BOUNCING:
		vy = -QBLOCK_BOUNCE_SPEED; 
		break;
	case QuestionBlockState::EMPTY:
	case QuestionBlockState::ACTIVE:
		vy = 0;
		break;
	}
}