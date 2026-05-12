#include "GameObject.h"
#include "SceneManager.h"

#define ID_ANI_COURSE_CLEAR_EFFECT 5030
#define SCREEN_WIDTH 270
#define SCREEN_HEIGHT 250

struct CourseClearPiece {
    int id;
    float offsetX;
    float offsetY;
};

const float STEP = 16.0f;


#pragma once
class CourseClearEffect : public GameObject 
{
    ULONGLONG start_timer;
    bool isDoneTally = false;

public:
    CourseClearEffect(float x, float y) : GameObject(x, y) 
    {
        start_timer = GetTickCount64();
    }

    void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) 
    {
        ULONGLONG elapsed = GetTickCount64() - start_timer;

        if (elapsed > 3000 && !isDoneTally) 
        {
            // Gọi logic cộng điểm và phát tiếng "tíc tíc" từ SoundManager
            isDoneTally = true;
        }

        // Giai đoạn cuối: Sau 5000ms thì chuyển Scene
        if (elapsed > 6000) 
        {
            SceneManager::GetInstance()->InitiateSwitchScene(8);
        }
    }

    void Render();
};