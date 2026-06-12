#include "KoopaTroopa.h"
#include "Koopa.h"
KoopaTroopa::KoopaTroopa(float x, float y) : Koopa(x, y)
{
	//SetState(KoopaState::WALKING);
	SetState(KoopaState::WING);
}

void KoopaTroopa::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (state == static_cast<int>(KoopaState::WING)) 
	{
		left = x - KOOPATROOPA_BBOX_WIDTH / 2;
		top = y - KOOPATROOPA_BBOX_HEIGHT / 2 + KOOPA_BBOX_OFFSET_Y;
		right = left + KOOPATROOPA_BBOX_WIDTH;
		bottom = top + KOOPATROOPA_BBOX_HEIGHT - 10;
		
	}
	else 
	{
		Koopa::GetBoundingBox(left, top, right, bottom);
	}
}



void KoopaTroopa::OnCollisionWith(LPCOLLISIONEVENT e)
{
	Koopa::OnCollisionWith(e);
	if (e->ny < 0 && state == static_cast<int>(KoopaState::WING)) 
	{
		DebugOut(L"Collision\n");
		vy = -KOOPA_PARATROOPA_BASE_FLYING_SPEED;
		DebugOut(L"vy = %f\n", vy);
	}
}

void KoopaTroopa::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	// Áp dụng trọng lực và gia tốc cho Koopa
	vy += ay * dt;
	vx += ax * dt;


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

void KoopaTroopa::Render()
{
	int aniId = -1;
	float renderX = x;
	float renderY = y; // Tọa độ y vật lý chuẩn chỉnh, không cần cộng bù trừ thủ công nữa
	if (state == static_cast<int>(KoopaState::WING))
	{
		aniId = ID_ANI_KOOPA_WING;
		
		Animations::GetInstance()->Get(aniId)->Render(renderX, renderY, isFlipped);
		// RenderBoundingBox();
		return;
	}
	else
		Koopa::Render();
}

void KoopaTroopa::SetState(KoopaState state)
{
	// Remember previous state so we can react to transitions (e.g., losing wings)
	KoopaState oldState = static_cast<KoopaState>(this->state);

	Koopa::SetState(state);

	// If we just transitioned from WING to WALKING, make the Koopa drop and lose its wings
	if (oldState == KoopaState::WING && this->state == static_cast<int>(KoopaState::WALKING))
	{
		// Start falling: ensure normal gravity and give an initial downward velocity
		ay = KOOPA_GRAVITY;
		vy = KOOPA_PARATROOPA_BASE_FLYING_SPEED; // positive = fall down
		DebugOut(L"KoopaTroopa::SetState: lost wings, start falling\n");
	}

	// Entering wing state: set upward velocity and slightly reduce gravity for floaty effect
	if (this->state == static_cast<int>(KoopaState::WING)) {
		DebugOut(L"KoopaParatroopa::SetState: state = KOOPA_PARATROOPA_STATE_FLYING\n");
		vy = -KOOPA_PARATROOPA_BASE_FLYING_SPEED;
		ay = KOOPA_GRAVITY / 1.4f;
	}
}



