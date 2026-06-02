#include "NoteBlock.h"
#include "PlayScene.h"
#include "Mario.h"
#include "Coin.h"
#include "Mushroom.h"
#include "Leaf.h"
#include "SoundManager.h"

NoteBlock::NoteBlock(float x, float y, int containedItem, int cnt) : GameObject(x, y)
{
    this->startY = y;
    this->currentState = NoteBlockState::ACTIVE;
    this->state = static_cast<int>(NoteBlockState::ACTIVE);
    this->vy = 0;
	this->bounceCount = cnt;

    switch (containedItem)
    {
        case static_cast<int>(ContainedItem::POWER_UP):
        {
            this->item = ContainedItem::POWER_UP;
            break;
        }
        case static_cast<int>(ContainedItem::UP_MUSHROOM):
        {

            this->item = ContainedItem::UP_MUSHROOM;
            break;
        }
        default:
        {
            this->item = ContainedItem::COIN;
            break;
        }
    }

    zIndex = 5;
}

void NoteBlock::GetBoundingBox(float& l, float& t, float& r, float& b)
{
    l = x - NOTE_BLOCK_BBOX_WIDTH / 2;
    t = y - NOTE_BLOCK_BBOX_HEIGHT / 2;
    r = l + NOTE_BLOCK_BBOX_WIDTH;
    b = t + NOTE_BLOCK_BBOX_HEIGHT;
}

void NoteBlock::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
    y += vy * dt;

    // Logic nảy 
    if (currentState == NoteBlockState::BOUNCING_UP)
    {
        if (startY - y >= NOTE_BLOCK_BOUNCE_HEIGHT && vy < 0)
        {
            vy = NOTE_BLOCK_BOUNCE_SPEED;
        }
        else if (y >= startY && vy > 0)
        {
            y = startY;
            vy = 0;
            if (bounceCount > 0)
            {
                if (item != ContainedItem::COIN) // Nếu có item kích hoạt
                {
                    SpawnItem();
                }
                else 
                {
                    SpawnItem(); // Coin ăn liên tục
                }
            }
            bounceCount--;
            SetState(NoteBlockState::ACTIVE);
        }
    }
    // Logic lún
    else if (currentState == NoteBlockState::BOUNCING_DOWN)
    {
        if (y - startY >= NOTE_BLOCK_BOUNCE_HEIGHT && vy > 0)
        {
            vy = -NOTE_BLOCK_BOUNCE_SPEED;
            MarioJumpDeflect(); // Đẩy Mario bắn lên ngay lúc hộp bắt đầu nảy ngược lại
        }
        else if (y <= startY && vy < 0)
        {
            y = startY;
            vy = 0;
            SetState(NoteBlockState::ACTIVE);
        }
    }
}

void NoteBlock::MarioJumpDeflect()
{
    PlayScene* playScene = (PlayScene*)SceneManager::GetInstance()->GetCurrentScene();
    Mario* mario = (Mario*)playScene->GetPlayer();

    if (mario != nullptr)
    {
        float mvx, mvy;
        mario->GetSpeed(mvx, mvy);
        mvx = mvx * 0.6f;

        if (mario->IsHoldingJump) 
        {
            mario->SetSpeed(mvx, -0.36f);
        }
        else
        {
            mario->SetSpeed(mvx, -0.2f);
        }
    }
}

void NoteBlock::SpawnItem()
{
    PlayScene* playScene = (PlayScene*)SceneManager::GetInstance()->GetCurrentScene();
    Mario* mario = (Mario*)playScene->GetPlayer();

    if (this->item == ContainedItem::COIN)
    {
        SoundManager::GetInstance()->Play("coin");
        Coin* coin = new Coin(x, y - 16);
        coin->SetState(CoinState::BOUNCING);
        playScene->AddObject(coin);
        mario->AddCoin();
    }
    else if (this->item == ContainedItem::POWER_UP)
    {
        if (mario->GetCurrentForm() == MarioForm::SMALL)
        {
            Mushroom* mushroom = new Mushroom(x, y); // Cho nhảy hẳn lên trên
            playScene->AddObject(mushroom);
        }
        else
        {
            Leaf* leaf = new Leaf(x, y);
            playScene->AddObject(leaf);
        }
    }
    else if (this->item == ContainedItem::UP_MUSHROOM)
    {
        OneUpMushroom* mushroom = new OneUpMushroom(x, y);
        playScene->AddObject(mushroom);
    }
}

void NoteBlock::Render()
{
    int aniId = ID_ANI_NOTE_BLOCK_ACTIVE;

    if (currentState == NoteBlockState::EMPTY)
    {
        //aniId = ID_ANI_NOTE_BLOCK_EMPTY;
    }

    Animations::GetInstance()->Get(aniId)->Render(x, y);
}

void NoteBlock::SetState(NoteBlockState state)
{
    this->currentState = state;
    switch (state)
    {
        case NoteBlockState::BOUNCING_UP:
            vy = -NOTE_BLOCK_BOUNCE_SPEED;
            SoundManager::GetInstance()->Play("bump");
            break;
        case NoteBlockState::BOUNCING_DOWN:
            vy = NOTE_BLOCK_BOUNCE_SPEED;
            SoundManager::GetInstance()->Play("bump");
            break;
        case NoteBlockState::ACTIVE:
            break;
        case NoteBlockState::EMPTY:
            //vy = 0;
            break;
    }
    GameObject::SetState(static_cast<int>(state));
}