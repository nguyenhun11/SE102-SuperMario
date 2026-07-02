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
	this->isRedBlock = false;
    wantsHighJump = false;

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

NoteBlock::NoteBlock(float x, float y, bool isRed, int targetSceneID) : NoteBlock(x, y, 0, 0)
{
    this->isRedBlock = isRed;
    this->targetSceneID = targetSceneID;
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
                SpawnItem();
            }
            bounceCount--;
            SetState(NoteBlockState::ACTIVE);
        }
    }

    else if (currentState == NoteBlockState::BOUNCING_DOWN)
    {
        PlayScene* playScene = (PlayScene*)SceneManager::GetInstance()->GetCurrentScene();
        Mario* mario = (Mario*)playScene->GetPlayer();

        InputManager* input = InputManager::GetInstance();
        if (input->IsKeyDown(DIK_SPACE) || input->IsKeyDown(DIK_Z))
        {
            this->wantsHighJump = true;
        }

        if (mario != nullptr)
        {
            float mx, my;
            mario->GetPosition(mx, my);

            mario->SetPosition(mx, this->y - 16.0f);

            float mvx, mvy;
            mario->GetSpeed(mvx, mvy);
            mario->SetSpeed(mvx, 0.0f);

            mario->SetIsOnPlatform(false);
        }

        if (vy > 0)
        {
            if (y - startY >= NOTE_BLOCK_BOUNCE_HEIGHT)
            {
                y = startY + NOTE_BLOCK_BOUNCE_HEIGHT;
                vy = -NOTE_BLOCK_BOUNCE_SPEED;
            }
        }
        else if (vy < 0)
        {
            if (y <= startY)
            {
                y = startY;
                vy = 0;
                SetState(NoteBlockState::ACTIVE);

                if (mario != nullptr)
                {
                    MarioJumpDeflect();
                }
            }
        }
    }
    else if (currentState == NoteBlockState::FLYING_TO_SKY)
    {
        vy = -NOTE_BLOCK_FLY_SPEED;

        if (y < -500.0f)
        {
            this->isDeleted = true; // Delete khỏi list
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

        mario->SetIsOnPlatform(false);

        if (this->wantsHighJump)
        {
            mario->SetState(MarioState::JUMP);
            mario->IsHoldingJump = true;

            if (this->isRedBlock)
            {
                mario->SetSpeed(0.0f, -NOTE_BLOCK_FLY_SPEED); // Bắn lên trời
                mario->StartFlyingToHeaven(this->targetSceneID);
            }
            else
            {
                mario->SetSpeed(mvx, -0.36f);
            }
        }
        else
        {
            mario->SetSpeed(mvx, -0.16f);
            mario->IsHoldingJump = false;
        }

        this->wantsHighJump = false;
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
    if (isRedBlock)
    {
        aniId = ID_ANI_NOTE_BLOCK_RED;
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