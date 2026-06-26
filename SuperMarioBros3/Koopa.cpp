#include "Koopa.h"
#include "Brick.h"
#include "Goomba.h"
#include "SoundManager.h"
#include "HitEffect.h"
#include "ScoreEffect.h"

Koopa::Koopa(float x, float y, KoopaColor color) : RespawnableEnemy(x, y)
{
	this->ax = 0;
	this->ay = KOOPA_GRAVITY;
	die_start = -1;
	isFlipped = false;
	isFlippedVertical = false;
	// Khởi tạo một sensor duy nhất đi trước để dò đường
	this->sensorfront = new Sensor(x, y);

	this->color = color;

	// Mặc định ban đầu đi sang trái
	PlayScene* scene = (PlayScene*)SceneManager::GetInstance()->GetCurrentScene();
	Mario* mario = (Mario*)scene->GetPlayer();
	if (mario != nullptr)
	{
		nx = (mario->GetX() > this->x) ? 1 : -1;
	}
	else
	{
		nx = -1;
	}
	SetState(KoopaState::WALKING);
	OnEnable();
}

void Koopa::OnEnable()
{
	this->isFlippedVertical = false;
	this->isHeld = false;
	this->vy = 0;
	this->state = static_cast<int>(KoopaState::WALKING);
	this->ay = KOOPA_GRAVITY;

	PlayScene* scene = (PlayScene*)SceneManager::GetInstance()->GetCurrentScene();
	Mario* mario = (Mario*)scene->GetPlayer();
	if (mario != nullptr)
	{
		if (color == KoopaColor::GREEN)
		{
			nx = (mario->GetX() > this->x) ? 1 : -1;
			vx = nx * KOOPA_WALKING_SPEED;
		}
	}
	else
	{
		nx = -1;
		vx = -KOOPA_WALKING_SPEED;
	}

	SetState(KoopaState::WALKING);
	if (sensorfront != nullptr) {
		float sensorOffsetX = nx * (KOOPA_BBOX_WIDTH / 2 + 2);
		float sensorOffsetY = KOOPA_BBOX_HEIGHT / 2 + 1;
		sensorfront->SetXY(x + sensorOffsetX, y + sensorOffsetY);
		sensorfront->SetHasGround(true);
	}
}

void Koopa::OnExitCamera()
{
	//if (state == (int)KoopaState::SHELL_MOVING)
	//{
	//	SetState(KoopaState::WALKING);
	//	RespawnableEnemy::OnExitCamera();
	//}
	//else
	//{
	//	RespawnableEnemy::OnExitCamera();
	//}
	RespawnableEnemy::OnExitCamera();
}

void Koopa::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (state == static_cast<int>(KoopaState::SHELL) || state == static_cast<int>(KoopaState::SHAKING) || state == static_cast<int>(KoopaState::SHELL_MOVING) || state == static_cast<int>(KoopaState::SHELL_UPWARD))
	{
		left = x - KOOPA_BBOX_WIDTH / 2;
		top = y - KOOPA_BBOX_HEIGHT_DIE / 2;
		right = left + KOOPA_BBOX_WIDTH;
		bottom = top + KOOPA_BBOX_HEIGHT_DIE;
	}
	else
	{
		left = x - KOOPA_BBOX_WIDTH / 2 + 1.0f;
		top = y - KOOPA_BBOX_HEIGHT / 2;
		right = left + KOOPA_BBOX_WIDTH - 2.0f;
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
	//if (!e->obj->IsBlocking()) return;
	//if (!e->obj->IsCollidable()) return;
	if (e->obj == this) return;

	if (e->ny != 0)
	{
		vy = 0;
		if (e->ny < 0 && state == static_cast<int>(KoopaState::SHELL_UPWARD))
		{
			SetState(KoopaState::SHELL); 
		}
	}
	else if (e->nx != 0)
	{
		nx = e->nx;

		if (state == static_cast<int>(KoopaState::SHELL_MOVING))
		{
			if (dynamic_cast<Goomba*>(e->obj))
			{
				OnCollisionWithGoomba(e);
				return;
			}
			else if (dynamic_cast<Koopa*>(e->obj))
			{
				OnCollisionWithKoopa(e);
				return;
			}
			else if (dynamic_cast<Brick*>(e->obj))
			{
				OnCollisionWithBrick(e);
			}
			else if (dynamic_cast<QuestionBlock*>(e->obj))
			{
				OnCollisionWithQuestionBlock(e);
			}
		}
		else if (state == static_cast<int>(KoopaState::WALKING))
		{
			if (dynamic_cast<Goomba*>(e->obj) || dynamic_cast<Koopa*>(e->obj))
			{
				nx = -nx;
				vx = nx * KOOPA_WALKING_SPEED;
			}
		}
		vx = nx * (state == static_cast<int>(KoopaState::SHELL_MOVING) ? KOOPA_SHELL_SPEED : KOOPA_WALKING_SPEED);
	}

	Koopa* koopa = dynamic_cast<Koopa*>(e->obj);
	if (koopa != NULL && koopa != this) return;
}

void Koopa::OnCollisionWithBrick(LPCOLLISIONEVENT e)
{
	Brick* brick = dynamic_cast<Brick*>(e->obj);
	if (brick == nullptr) return; 

	if (brick->GetCurrentState() == BrickState::ACTIVE)
	{
		if (brick->GetContainedItem() != BrickItem::NONE)
		{
			brick->SetState(BrickState::BOUNCING);
			SoundManager::GetInstance()->Play("bump");
		}
		else
		{
			brick->Break();
		}
	}
}

void Koopa::OnCollisionWithQuestionBlock(LPCOLLISIONEVENT e)
{
	QuestionBlock* qb = dynamic_cast<QuestionBlock*>(e->obj);
	if (qb == nullptr) return; // Áo giáp chống crash

	if (qb->GetCurrentState() == QuestionBlockState::ACTIVE)
	{
		// Khối chấm hỏi luôn nảy lên
		qb->SetState(QuestionBlockState::BOUNCING);
	}
}

void Koopa::OnCollisionWithGoomba(LPCOLLISIONEVENT e)
{
	Goomba* goomba = dynamic_cast<Goomba*>(e->obj);
	if (goomba == nullptr) return;

	PlayScene* scene = dynamic_cast<PlayScene*>(SceneManager::GetInstance()->GetCurrentScene());
	if (goomba->GetState() == static_cast<int>(GoombaState::WALKING))
	{
		goomba->SetState(GoombaState::BOUNCE);
		HitEffect* effect = new HitEffect(goomba->GetX(), goomba->GetY());
		scene->AddObject(effect);
		ScoreEffect* scoreEff = new ScoreEffect(goomba->GetX(), goomba->GetY(), Score::ONE_HUNDRED);
		scene->AddObject(scoreEff);
		GameManager::GetInstance()->AddScore(100);
	}
}

void Koopa::OnCollisionWithKoopa(LPCOLLISIONEVENT e)
{
	Koopa* koopa = dynamic_cast<Koopa*>(e->obj);
	if (koopa == nullptr) return;

	PlayScene* scene = dynamic_cast<PlayScene*>(SceneManager::GetInstance()->GetCurrentScene());
	if (koopa->GetState() != static_cast<int>(KoopaState::DIE) && koopa->GetState() != static_cast<int>(KoopaState::SHELL) && koopa->GetState() != static_cast<int>(KoopaState::SHAKING))
	{
		koopa->isFlippedVertical = true;
		koopa->SetState(KoopaState::DIE);
		HitEffect* effect = new HitEffect(koopa->GetX(), koopa->GetY());
		scene->AddObject(effect);
		ScoreEffect* scoreEff = new ScoreEffect(koopa->GetX(), koopa->GetY(), Score::ONE_HUNDRED);
		scene->AddObject(scoreEff);
		GameManager::GetInstance()->AddScore(100);
	}
}

void Koopa::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (isHeld)
	{
		vx = 0.0f;
		vy = 0.0f;
		ax = 0.0f;
		ay = 0.0f;

		// đếm thời gian rung lắc và tỉnh dậy
		if ((this->state == static_cast<int>(KoopaState::SHELL) || this->state == static_cast<int>(KoopaState::SHELL_UPWARD)) && (GetTickCount64() - die_start > KOOPA_DIE_TIMEOUT - 200))
		{
			SetState(KoopaState::SHAKING); 
		}
		if ((this->state == static_cast<int>(KoopaState::SHAKING)) && (GetTickCount64() - die_start > KOOPA_DIE_TIMEOUT))
		{
			SetState(KoopaState::WALKING); 
				isHeld = false; 
		}
		return;
	}

	// Áp dụng trọng lực và gia tốc cho Koopa
	vy += ay * dt;
	vx += ax * dt;

	// === XỬ LÝ SENSOR CHO TỪNG TRẠNG THÁI ===
	if (sensorfront != nullptr)
	{
		//if (state == static_cast<int>(KoopaState::WALKING))
		//{
		//	// Đặt vị trí Sensor nhô ra phía trước chân của Koopa theo hướng di chuyển (nx)
		//	float sensorOffsetX = nx * (KOOPA_BBOX_WIDTH / 2 + 2);
		//	float sensorOffsetY = KOOPA_BBOX_HEIGHT / 2; // Nằm sát mép dưới chân

		//	sensorfront->SetXY(x + sensorOffsetX, y + sensorOffsetY);
		//	sensorfront->Update(dt, coObjects);

		//	// CHỈ quay đầu nếu Koopa đang thực sự di chuyển (vx != 0) và là Koopa đỏ
		//	if (this->color == KoopaColor::RED)
		//	{
		//		if (vx != 0 && sensorfront->IsFalling())
		//		{
		//			vx = -vx;
		//			nx = -nx;

		//			sensorOffsetX = nx * (KOOPA_BBOX_WIDTH / 2 + 2);
		//			sensorfront->SetXY(x + sensorOffsetX, y + sensorOffsetY);
		//		}
		//	}
		//}
		if (state == static_cast<int>(KoopaState::WALKING))
		{
			float checkX = x + nx * (KOOPA_BBOX_WIDTH / 2.0f - 2.0f);
			float feetY = y + KOOPA_BBOX_HEIGHT / 2.0f;

			bool groundAhead = false;
			for (auto obj : *coObjects)
			{
				if (!obj->IsBlocking()) continue;

				float ol, ot, or_, ob;
				obj->GetBoundingBox(ol, ot, or_, ob);

				if (checkX < ol || checkX > or_) continue;

				if (ot <= feetY + 4.0f && ot >= feetY - 4.0f)
				{
					groundAhead = true;
					break;
				}
			}

			if (this->color == KoopaColor::RED)
			{
				if (vx != 0 && !groundAhead)
				{
					vx = -vx;
					nx = -nx;
				}
			}

			// Vẫn update sensor cho render debug
			if (sensorfront != nullptr)
			{
				float sensorOffsetX = nx * (KOOPA_BBOX_WIDTH / 2 + 2);
				float sensorOffsetY = KOOPA_BBOX_HEIGHT / 2;
				sensorfront->SetXY(x + sensorOffsetX, y + sensorOffsetY);
				sensorfront->Update(dt, coObjects);
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
	if ((this->state == static_cast<int>(KoopaState::SHELL) || this->state == static_cast<int>(KoopaState::SHELL_UPWARD)) && (GetTickCount64() - die_start > KOOPA_DIE_TIMEOUT - 1000)) // Rung lắc trước khi tỉnh lại 200ms
	{
		SetState(KoopaState::SHAKING);
	}
	if ((this->state == static_cast<int>(KoopaState::SHAKING)) && (GetTickCount64() - die_start > KOOPA_DIE_TIMEOUT))
	{
		SetState(KoopaState::WALKING);
		return;
	}

	// Gọi cập nhật va chạm vật lý Swept AABB tiêu chuẩn cho Koopa
	RespawnableEnemy::Update(dt, coObjects);
	Collision::GetInstance()->Process(this, dt, coObjects);
}

void Koopa::Render()
{
	int aniId = -1;
	float renderX = x;
	float renderY = y; // Tọa độ y vật lý chuẩn chỉnh, không cần cộng bù trừ thủ công nữa

	if (this->state == static_cast<int>(KoopaState::DIE))
	{
		if (color == KoopaColor::GREEN)
			aniId = ID_ANI_KOOPA_GREEN_SHELL;
		else
			aniId = ID_ANI_KOOPA_RED_SHELL;
	}
	else if (this->state == static_cast<int>(KoopaState::SHELL))
	{
		if (color == KoopaColor::GREEN)
			aniId = ID_ANI_KOOPA_GREEN_SHELL;
		else
			aniId = ID_ANI_KOOPA_RED_SHELL;
	}
	else if (this->state == static_cast<int>(KoopaState::SHELL_UPWARD))
	{
		if (color == KoopaColor::GREEN)
			aniId = ID_ANI_KOOPA_GREEN_SHELL;
		else
			aniId = ID_ANI_KOOPA_RED_SHELL;
	}
	else if (this->state == static_cast<int>(KoopaState::WALKING))
	{
		if (color == KoopaColor::GREEN)
			aniId = ID_ANI_KOOPA_GREEN_WALKING;
		else
			aniId = ID_ANI_KOOPA_RED_WALKING;
	}
	else if (this->state == static_cast<int>(KoopaState::SHELL_MOVING))
	{
		if (color == KoopaColor::GREEN)
			aniId = ID_ANI_KOOPA_GREEN_SHELL_MOVING;
		else
			aniId = ID_ANI_KOOPA_RED_SHELL_MOVING;
	}
	else if (this->state == static_cast<int>(KoopaState::SHAKING))
	{
		if (color == KoopaColor::GREEN)
			aniId = ID_ANI_KOOPA_GREEN_SHELL_SHAKING;
		else
			aniId = ID_ANI_KOOPA_RED_SHELL_SHAKING;

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

	if (aniId == -1)
	{
		if (color == KoopaColor::GREEN)
			aniId = ID_ANI_KOOPA_GREEN_WALKING;
		else
			aniId = ID_ANI_KOOPA_RED_WALKING;
	}

	// Lật ảnh sprite (isFlip = true nếu đi sang phải)
	isFlipped = (nx > 0);

	LPANIMATION ani = Animations::GetInstance()->Get(aniId);
	if (ani == NULL)
	{
		DebugOut(L"[ERROR] Koopa khong tim thay Animation ID: %d \n", aniId);
		return; // Bỏ qua render frame này để không bị văng game
	}

	ani->Render(renderX, renderY, isFlipped, isFlippedVertical);

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
	case KoopaState::DIE:
		this->ay = KOOPA_GRAVITY;
		this->vy = -0.35f;
		this->vx = this->nx * 0.05f;
		this->isFlippedVertical = true;
		// nếu đang bị Mario cầm thì tự động giải thoát cho rùa rơi tự do
		this->isHeld = false;
		break;
	case KoopaState::SHELL:
		die_start = GetTickCount64();
		ay = KOOPA_GRAVITY;
		vx = 0;
		vy = 0;

		// DỊCH CHUYỂN Y VẬT LÝ XUỐNG DƯỚI: Hạ tâm Y xuống sát đất do BBox bị thu nhỏ lại
		if (oldState == KoopaState::WALKING || oldState == KoopaState::SHAKING || oldState == KoopaState::SHELL_UPWARD)
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

		if (isFlippedVertical)
		{
			isFlippedVertical = false;
		}

		// TRẢ LẠI Y VẬT LÝ KHI ĐỨNG LÊN: Đẩy tâm Y lên trên để tránh chân Koopa lún sâu vào gạch
		if (oldState == KoopaState::SHELL || oldState == KoopaState::SHAKING || oldState == KoopaState::SHELL_MOVING || oldState == KoopaState::SHELL_UPWARD)
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

	case KoopaState::SHELL_UPWARD:
		die_start = GetTickCount64(); // Reset lại thời gian để không bị hồi sinh ngay lập tức

		this->ay = KOOPA_GRAVITY;
		this->vx = nx * KOOPA_WALKING_SPEED * 1.5f; 
		this->vy = -KOOPA_SHELL_SPEED * 2.f;      

		isFlippedVertical = true;
		if (oldState == KoopaState::WALKING || oldState == KoopaState::SHAKING)
		{
			y += (KOOPA_BBOX_HEIGHT - KOOPA_BBOX_HEIGHT_DIE) / 2;
		}

		if (sensorfront != nullptr) {
			sensorfront->SetXY(x, y);
			sensorfront->SetHasGround(true);
		}
		break;
	}


}

void Koopa::SetDirection(int d)
{
	nx = d;
}