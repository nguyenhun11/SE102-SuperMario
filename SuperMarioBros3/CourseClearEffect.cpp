#include "CourseClearEffect.h"
#include "PlayScene.h"


void CourseClearEffect::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
    ULONGLONG elapsed = GetTickCount64() - start_timer;
    PlayScene* scene = (PlayScene*)SceneManager::GetInstance()->GetCurrentScene();



    // sau 1.5 giây thì hiện COURSE CLEAR
    if (elapsed > 1500 && !isSpawnedClearText)
    {
        scene->AddObject(new CourseClearText(x * 16 - 4, y * 16 - 60));
        isSpawnedClearText = true;
    }

    // sau 2.5s =>  YOU GOT A CARD và hình THẺ BÀI ra
    if (elapsed > 2500 && !isSpawnedCardText && !isSpawnedCardSprite)
    {
        scene->AddObject(new YouGotCardText(x * 16 - 20, y * 16 + 20.0 - 60));
        scene->AddObject(new GoalCardEffect(x * 16 + 84.0f - 20, y * 16 + 20.0f - 60, card));
        isSpawnedCardText = true;
        isSpawnedCardSprite = true;
    }

    // sau 3.5s thì đếm điểm
    if (elapsed > 3500 && !isDoneTally)
    {
        // NOTE: logic nhảy điểm
        isDoneTally = true;
    }

    // Chuyển Scene
    if (isDoneTally && elapsed > 6000)
    {
        SceneManager::GetInstance()->InitiateSwitchScene(0);
    }
}