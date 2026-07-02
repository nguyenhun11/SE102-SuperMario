#pragma once
#include "GameObject.h"
#include "Brick.h"
#include "QuestionBlock.h"

class Sensor : public GameObject
{
private:
    bool hasGround;

public:
    Sensor(float x, float y) : GameObject(x, y)
    {
        hasGround = true;
    }

    void SetXY(float x, float y)
    {
        this->x = x;
        this->y = y;
    }

    virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom)
    {
        // Tạo một chiếc hộp kiểm tra nhỏ (kích thước 2x4) thọc xuống dưới chân Koopa
        left = x - 4;
        top = y;
        right = left + 8;
        // tăng chiều sâu để tránh bỏ sót khi bề mặt mỏng
        bottom = top + 10;
    }

    virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
    {
        // Mặc định mỗi frame coi như không có đất, trừ khi tìm thấy va chạm bên dưới
        hasGround = false;

        float sL, sT, sR, sB;
        GetBoundingBox(sL, sT, sR, sB);

        // Duyệt danh sách các vật thể trong map để kiểm tra giao nhau trực tiếp (AABB Overlap)
        for (UINT i = 0; i < coObjects->size(); i++)
        {
            LPGAMEOBJECT obj = coObjects->at(i);

            // Bỏ qua chính nó, bỏ qua Koopa và các vật thể không có thuộc tính chặn (IsBlocking)
            if (obj == this || !obj->IsBlocking()) continue;

            float oL, oT, oR, oB;
            obj->GetBoundingBox(oL, oT, oR, oB);

            // Thuật toán kiểm tra Overlap giữa hộp Sensor và vật thể nền đất
            if (sL < oR && sR > oL && sT < oB && sB > oT)
            {
                // Nếu là vật thể có tính chất chặn (IsBlocking), coi đó là mặt đất
                // (Tránh kiểm tra bằng dynamic_cast vì có nhiều loại mặt đất: SolidBlock, Ground, Platform, ...)
                if (obj->IsBlocking())
                {
                    hasGround = true;
                    break; // Đã tìm thấy đất đỡ, dừng vòng lặp sớm để tối ưu
                }
            }
        }
    }

    virtual void Render()
    {
        //RenderBoundingBox();
    }

    // Các hàm thuần ảo bắt buộc của GameObject nhưng không dùng logic vật lý Swept AABB cho Sensor
    virtual int IsCollidable() { return 0; }
    virtual int IsBlocking() { return 0; }
    virtual void OnNoCollision(DWORD dt) {}
    virtual void OnCollisionWith(LPCOLLISIONEVENT e) {}

    // Hàm trả về trạng thái rơi (không tìm thấy đất bên dưới chân)
    bool IsFalling() { return !hasGround; }

    // Force sensor ground state (useful when repositioning sensor to avoid one-frame false fall)
    void SetHasGround(bool v) { hasGround = v; }
};