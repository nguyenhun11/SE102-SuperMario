#include "QuestionBlock.h"
#include "PlayScene.h"
#include "Coin.h"
#include "Mushroom.h"
//#include "Leaf.h"
//#include "OneUpMushroom.h"
#include "debug.h"

QuestionBlock::QuestionBlock(float x, float y, int containedItem) : GameObject(x, y)
{
	this->startY = y;
	this->currentState = QuestionBlockState::ACTIVE;
	this->state = static_cast<int>(QuestionBlockState::ACTIVE);
	this->vy = 0;

	switch (containedItem)
	{
		case static_cast<int>(ContainedItem::POWER_UP):
		{
			this->item = ContainedItem::POWER_UP;
			break;
		}
		default:
		{
			this->item = ContainedItem::COIN;
			break;
		}
	}

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

			// sinh ra item khối này đang đựng
			SpawnItem();

		}
	}
}

void QuestionBlock::SpawnItem()
{
	PlayScene* playScene = (PlayScene*)SceneManager::GetInstance()->GetCurrentScene();
	Mario* mario = (Mario*)playScene->GetPlayer();

	if (this->item == ContainedItem::COIN)
	{
		// thêm object vào scene đang chơi
		Coin* coin = new Coin(x, y - 16); // trừ 16 đơn vị độ cao
		coin->SetState(CoinState::BOUNCING);
		playScene->AddObject(coin);
		mario->AddCoin();
	}
	// logic để sinh ra các power up dựa treenn state hieennnj tại của thằng mairo
	else if (this->item == ContainedItem::POWER_UP)
	{
		if (mario->GetCurrentForm() == MarioForm::SMALL)
		{
			// giữ nguyên tọa độ y
			Mushroom* mushroom = new Mushroom(x, y);
			playScene->AddObject(mushroom);
		}
		else if(mario->GetCurrentForm() == MarioForm::SUPER)
		{
			// sinh đồ xịn honw nếu con mario đang ở trạng thái TO
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

	GameObject::SetState(static_cast<int>(state));
}