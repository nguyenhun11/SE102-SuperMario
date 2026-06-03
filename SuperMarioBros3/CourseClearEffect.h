#include "GameObject.h"
#include "SceneManager.h"
#include "GoalBlock.h"

#define ID_ANI_COURSE_CLEAR_EFFECT 5030
#define ID_ANI_YOU_GOT_CARD 5031
#define SCREEN_WIDTH 270
#define SCREEN_HEIGHT 250
#define ID_ANI_CARD_MUSHROOM_EFFECT 5032
#define ID_ANI_CARD_FLOWER_EFFECT 5033
#define ID_ANI_CARD_STAR_EFFECT 5034

struct CourseClearPiece {
    int id;
    float offsetX;
    float offsetY;
};

const float STEP = 16.0f;


#pragma once
//class CourseClearEffect : public GameObject 
//{
//    ULONGLONG start_timer;
//    bool isDoneTally = false;
//
//public:
//    CourseClearEffect(float x, float y) : GameObject(x, y) 
//    {
//        start_timer = GetTickCount64();
//    }
//
//    void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) 
//    {
//        ULONGLONG elapsed = GetTickCount64() - start_timer;
//
//        if (elapsed > 3000 && !isDoneTally) 
//        {
//            // Gọi logic cộng điểm và phát tiếng "tíc tíc" từ SoundManager
//            isDoneTally = true;
//        }
//
//        // Giai đoạn cuối: Sau 5000ms thì chuyển Scene
//        if (elapsed > 6000) 
//        {
//            SceneManager::GetInstance()->InitiateSwitchScene(8);
//        }
//    }
//
//    void Render();
//};

class CourseClearEffect : public GameObject
{
    ULONGLONG start_timer;
    CardType card;

    bool isSpawnedClearText = false;
    bool isSpawnedCardText = false;
    bool isSpawnedCardSprite = false;
    bool isDoneTally = false;

public:
    CourseClearEffect(float x, float y, CardType cardType) : GameObject(x, y)
    {
        start_timer = GetTickCount64();
        card = cardType;
    }
    void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
    void Render() {}
    void GetBoundingBox(float& l, float& t, float& r, float& b) { l = t = r = b = 0; }
};


class CourseClearText : public GameObject 
{
public:
    CourseClearText(float x, float y) : GameObject(x, y) {}
    void Render() override 
    {
        Animations::GetInstance()->Get(ID_ANI_COURSE_CLEAR_EFFECT)->Render(x, y);
    }
    void GetBoundingBox(float& l, float& t, float& r, float& b) override { l = t = r = b = 0; }
};

class YouGotCardText : public GameObject 
{

public:
    YouGotCardText(float x, float y) : GameObject(x, y) {}

    void Render() override 
    {
        Animations::GetInstance()->Get(ID_ANI_YOU_GOT_CARD)->Render(x, y); 
    }

    void GetBoundingBox(float& l, float& t, float& r, float& b) override { l = t = r = b = 0; }
};


class GoalCardEffect : public GameObject 
{
private:
    CardType card;

public:
    GoalCardEffect(float x, float y, CardType type) : GameObject(x, y) { card = type; }

    void Render() override 
    {
        int aniId = ID_ANI_CARD_MUSHROOM_EFFECT; 
        if (card == CardType::FLOWER) aniId = ID_ANI_CARD_FLOWER_EFFECT;
        else if (card == CardType::STAR) aniId = ID_ANI_CARD_STAR_EFFECT;
        Animations::GetInstance()->Get(aniId)->Render(x, y);
    }

    void GetBoundingBox(float& l, float& t, float& r, float& b) override { l = t = r = b = 0; }
};