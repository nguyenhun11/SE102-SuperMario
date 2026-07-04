#include "Switch.h"
#include "Animations.h"

Switch::Switch(float x, float y, SwitchType type) : GameObject(x, y)
{
	this->type = type;
	this->startY = y;
	this->zIndex = 4; // Để render đè lên trên viên gạch
	SetState((int)SwitchState::Spawning);
}

void Switch::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	// Nếu đang trồi lên
	if (state == (int)SwitchState::Spawning)
	{
		y += vy * dt;

		// Kiểm tra nếu đã trồi lên đủ 1 ô gạch (16px)
		if (startY - y >= 16.0f)
		{
			y = startY - 16.0f; // Snap cứng lại cho chuẩn pixel
			SetState((int)SwitchState::Off); // Đổi thành trạng thái nằm im chờ Mario đạp
		}
	}
}

void Switch::Render()
{
	int aniId = -1;

	// Xử lý ảnh tuỳ theo Type (Xanh nhạt / Xanh đậm)
	if (type == SwitchType::BrickToCoin)
	{
		aniId = (state == (int)SwitchState::On) ? BRICK_TO_COIN_ON_ANI_ID : BRICK_TO_COIN_OFF_ANI_ID;
	}
	else
	{
		aniId = (state == (int)SwitchState::On) ? COIN_TO_BRICK_ON_ANI_ID : COIN_TO_BRICK_OFF_ANI_ID;
	}

	Animations::GetInstance()->Get(aniId)->Render(x, y);
}

void Switch::SetState(int state)
{
	switch ((SwitchState)state)
	{
	case SwitchState::Spawning:
		vy = -SWITCH_SPAWN_SPEED;
		vx = 0;
		break;
	case SwitchState::Off:
		vy = 0;
		vx = 0;
		break;
	case SwitchState::On:
		vy = 0;
		vx = 0;
		// (Tuỳ chọn) Khi bẹp xuống có thể chỉnh toạ độ y thấp xuống 1 xíu
		// để khớp với animation công tắc xẹp xuống
		break;
	}

	GameObject::SetState(state);
}

void Switch::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	// Thông số bbox giả định, Hưn căn chỉnh lại theo kích thước ảnh thực tế nhé
	l = x - 8.0f;
	t = y - 8.0f;
	r = l + 16.0f;
	b = t + 16.0f;

	// Khi bị đạp (On), bóp chiều cao bounding box lại
	if (state == (int)SwitchState::On)
	{
		t += 8.0f; // Bẹp xuống một nửa
	}
}