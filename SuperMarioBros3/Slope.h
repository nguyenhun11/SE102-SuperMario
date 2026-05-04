#include "GameObject.h"

#pragma once
// Slope.h
class Slope : public GameObject
{
public:
    int direction; // 1: dốc lên, -1: dốc xuống
    float width, height;

    Slope(float x, float y, float w, float h, int dir) : GameObject(x, y)
    {
        width = w;
        height = h;
        direction = dir;
    }

    void Render();
    void GetBoundingBox(float& l, float& t, float& r, float& b);

    int IsBlocking() { return 0; }
    int IsCollidable() { return 1; }

    // map tọa độ Y dựa trên x mario
    float GetSurfaceY(float marioX);
};