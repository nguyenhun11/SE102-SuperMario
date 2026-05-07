#include "Slope.h"

float Slope::GetSurfaceY(float marioX)
{
    float leftX = x - width / 2; // mép trái
    float rightX = x + width / 2; // mép phải

    // set vị trí mario không vượt quá tile
    if (marioX < leftX) marioX = leftX;
    if (marioX > rightX) marioX = rightX;

    // tỉ lệ đã đi trên dốc
    float percent = (marioX - leftX) / width;

    // T
    float surfaceY = 0;
    if (direction == 1) // '/'
    {
        surfaceY = y + (height / 2) - (percent * height);
    }
    else if (direction == -1) // '\'
    {
        surfaceY = y - (height / 2) + (percent * height);
    }
    return surfaceY;
}

void Slope::Render()
{
    RenderBoundingBox();
}

void Slope::GetBoundingBox(float& l, float& t, float& r, float& b)
{
    l = x - width / 2.0f;
    t = y - height / 2.0f;
    r = l + width;
    b = t + height;
}