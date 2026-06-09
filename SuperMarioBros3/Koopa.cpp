#include "Koopa.h"
#include "Brick.h"
#include "Goomba.h"

Koopa::Koopa(float x, float y) : GameObject(x, y)
{
	this->ax = 0;
	this->ay = KOOPA_GRAVITY;
	die_start = -1;

	// Khởi tạo một sensor duy nhất đi trước để dò đường
	this->sensorfront = new Sensor(x, y);
	this->sensorback = nullptr;

	// Mặc định ban đầu đi sang trái
	this->nx = -1;
	SetState(KoopaState::WALKING);
}

void Koopa::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (state == static_cast<int>(KoopaState::SHELL) || state == static_cast<int>(KoopaState::SHAKING) || state == static_cast<int>(KoopaState::SHELL_MOVING))
	{
		left = x - KOOPA_BBOX_WIDTH / 2;
		top = y - KOOPA_BBOX_HEIGHT_DIE / 2;
		right = left + KOOPA_BBOX_WIDTH;
		bottom = top + KOOPA_BBOX_HEIGHT_DIE;
	}
	else
	{
		left = x - KOOPA_BBOX_WIDTH / 2;
		top = y - KOOPA_BBOX_HEIGHT / 2;
		right = left + KOOPA_BBOX_WIDTH;
		bottom = top + KOOPA_BBOX_HEIGHT;
	}
}

void Koopa::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
}

void Koopa::OnCollisionWith(LPCOLLISIONEVENT e)
{
	if (!e->obj->IsBlocking()) return;
	if (dynamic_cast<Koopa*>(e->obj)) return;

	if (e->ny != 0)
	{
		vy = 0;
	}
	else if (e->nx != 0)
	{
		// Va chạm tường đứng thì đổi hướng di chuyển ngược lại dựa vào hướng va chạm
		nx = e->nx;
		vx = nx * (state == static_cast<int>(KoopaState::SHELL_MOVING) ? KOOPA_SHELL_SPEED : KOOPA_WALKING_SPEED);
	}
}

void Koopa::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	// Áp dụng trọng lực và gia tốc cho Koopa
	vy += ay * dt;
	vx += ax * dt;

	// === XỬ LÝ SENSOR CHO TỪNG TRẠNG THÁI ===
	if (sensorfront != nullptr)
	{
		if (state == static_cast<int>(KoopaState::WALKING))
		{
			// Đặt vị trí Sensor nhô ra phía trước chân của Koopa theo hướng di chuyển (nx)
			float sensorOffsetX = nx * (KOOPA_BBOX_WIDTH / 2 + 2);
			float sensorOffsetY = KOOPA_BBOX_HEIGHT / 2; // Nằm sát mép dưới chân

			sensorfront->SetXY(x + sensorOffsetX, y + sensorOffsetY);
			sensorfront->Update(dt, coObjects);

			// CHỈ quay đầu nếu Koopa đang thực sự di chuyển (vx != 0)
			if (vx != 0 && sensorfront->IsFalling())
			{
				vx = -vx;
				nx = -nx;

				// Cập nhật lại vị trí Sensor ngay sang rìa bên kia để tránh lặp logic ở frame kế tiếp
				sensorOffsetX = nx * (KOOPA_BBOX_WIDTH / 2 + 2);
				sensorfront->SetXY(x + sensorOffsetX, y + sensorOffsetY);
			}
		}
		else // Các trạng thái SHELL, SHAKING, SHELL_MOVING
		{
			// Khóa chặt Sensor đi theo tâm trục tọa độ của Mai rùa
			sensorfront->SetXY(x, y);
			sensorfront->SetHasGround(true); // Ép trạng thái có nền đất để tránh báo lỗi hụt chân sai vị trí
			sensorfront->Update(dt, coObjects);
		}
	}

	// Xử lý chuyển đổi trạng thái dựa trên thời gian (Mai rùa -> Rung lắc -> Sống lại)
	if ((this->state == static_cast<int>(KoopaState::SHELL)) && (GetTickCount64() - die_start > KOOPA_DIE_TIMEOUT - 200)) // Rung lắc trước khi tỉnh lại 200ms
	{
		SetState(KoopaState::SHAKING);
	}
	if ((this->state == static_cast<int>(KoopaState::SHAKING)) && (GetTickCount64() - die_start > KOOPA_DIE_TIMEOUT))
	{
		SetState(KoopaState::WALKING);
		return;
	}

	// Gọi cập nhật va chạm vật lý Swept AABB tiêu chuẩn cho Koopa
	GameObject::Update(dt, coObjects);
	Collision::GetInstance()->Process(this, dt, coObjects);
}

void Koopa::Render()
{
	int aniId = -1;
	float renderX = x;
	float renderY = y; // Tọa độ y vật lý chuẩn chỉnh, không cần cộng bù trừ thủ công nữa

	if (this->state == static_cast<int>(KoopaState::SHELL))
	{
		aniId = ID_ANI_KOOPA_SHELL;
	}
	else if (this->state == static_cast<int>(KoopaState::WALKING))
	{
		aniId = ID_ANI_KOOPA_WALKING;
	}
	else if (this->state == static_cast<int>(KoopaState::SHELL_MOVING))
	{
		aniId = ID_ANI_KOOPA_SHELL_MOVING;
	}
	else if (this->state == static_cast<int>(KoopaState::SHAKING))
	{
		aniId = ID_ANI_KOOPA_SHELL_SHAKING;

		// Hiệu ứng rung lắc nhẹ bằng toán tử Modulo thời gian
		if ((GetTickCount64() / 50) % 2 == 0)
		{
			renderX += 1.0f;
		}
		else
		{
			renderX -= 1.0f;
		}
	}

	if (aniId == -1) aniId = ID_ANI_KOOPA_WALKING;

	// Lật ảnh sprite (isFlip = true nếu đi sang phải)
	bool isFlip = (nx > 0);
	Animations::GetInstance()->Get(aniId)->Render(renderX, renderY, isFlip);

	// Chỉ Render Sensor để theo dõi trực quan khi đang đi bộ
	if (state == static_cast<int>(KoopaState::WALKING) && sensorfront != nullptr)
	{
		sensorfront->Render();
	}
}

void Koopa::SetState(KoopaState state)
{
	KoopaState oldState = static_cast<KoopaState>(this->state);
	GameObject::SetState(static_cast<int>(state));

	switch (state)
	{
	case KoopaState::SHELL:
		die_start = GetTickCount64();
		ay = KOOPA_GRAVITY;
		vx = 0;
		vy = 0;

		// DỊCH CHUYỂN Y VẬT LÝ XUỐNG DƯỚI: Hạ tâm Y xuống sát đất do BBox bị thu nhỏ lại
		if (oldState == KoopaState::WALKING || oldState == KoopaState::SHAKING)
		{
			y += (KOOPA_BBOX_HEIGHT - KOOPA_BBOX_HEIGHT_DIE) / 2;
		}

		if (sensorfront != nullptr) {
			sensorfront->SetXY(x, y);
			sensorfront->SetHasGround(true);
		}
		break;

	case KoopaState::WALKING:
		this->ay = KOOPA_GRAVITY;
		vx = nx * KOOPA_WALKING_SPEED;
		vy = 0;

		// TRẢ LẠI Y VẬT LÝ KHI ĐỨNG LÊN: Đẩy tâm Y lên trên để tránh chân Koopa lún sâu vào gạch
		if (oldState == KoopaState::SHELL || oldState == KoopaState::SHAKING || oldState == KoopaState::SHELL_MOVING)
		{
			y -= (KOOPA_BBOX_HEIGHT - KOOPA_BBOX_HEIGHT_DIE) / 2;
		}

		if (sensorfront != nullptr) {
			float sensorOffsetX = nx * (KOOPA_BBOX_WIDTH / 2 + 2);
			float sensorOffsetY = KOOPA_BBOX_HEIGHT / 2 + 1;
			sensorfront->SetXY(x + sensorOffsetX, y + sensorOffsetY);
			sensorfront->SetHasGround(true);
		}
		else {
			sensorfront = new Sensor(x, y);
		}
		break;

	case KoopaState::SHELL_MOVING:
		this->ay = KOOPA_GRAVITY;
		// Chạy theo hướng nx được đồng bộ (từ Mario hoặc đập tường nảy ra)
		vx = nx * KOOPA_SHELL_SPEED;

		if (sensorfront != nullptr) {
			sensorfront->SetHasGround(true);
		}
		break;

	case KoopaState::SHAKING:
		vx = 0;
		vy = 0;
		break;
	}
}