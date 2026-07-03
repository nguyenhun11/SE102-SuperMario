#include "RespawnableEnemy.h"
#include "PlayScene.h"
#include "SceneManager.h"
#include "Mario.h"

#pragma region Respawnable Enemy


RespawnableEnemy::RespawnableEnemy(float x, float y) : GameObject(x, y)
{
	respawnPoint = new RespawnPoint(x, y, this);

	PlayScene* scene = dynamic_cast<PlayScene*>(SceneManager::GetInstance()->GetCurrentScene());
	if (scene != nullptr) {
		scene->AddObject(respawnPoint);
	}

	ax = 0.0f;
	ay = 0.0f;

	isHiddenOnPause = true;
}

void RespawnableEnemy::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{}

void RespawnableEnemy::OnExitCamera()
{
	float px, py;
	respawnPoint->GetPosition(px, py);
	SetPosition(px, py);
	SetActive(false);
}

void RespawnableEnemy::Delete()
{
	if (respawnPoint != nullptr)
	{
		respawnPoint->Delete();
		respawnPoint = nullptr;
	}
	GameObject::Delete();
}

void RespawnableEnemy::OnAttackedByTail(float direction)
{
	// Dùng sau này khi Mario quất đuôi
}

void RespawnableEnemy::SetState(int state)
{
	GameObject::SetState(state);
}

bool RespawnableEnemy::IsOtherEnemyOrMario(LPGAMEOBJECT obj)
{
	if (dynamic_cast<Mario*>(obj) != nullptr) return true;
	if (dynamic_cast<RespawnableEnemy*>(obj) != nullptr) return true;
	return false;
}
#pragma endregion

#pragma region Respawn Point

RespawnPoint::RespawnPoint(float x, float y, RespawnableEnemy* obj) : GameObject(x, y)
{
	this->obj = obj;
	if (!IsOnCamera()) {
		obj->SetActive(false);
	}
}

void RespawnPoint::OnEnterCamera()
{
	if (obj != nullptr && !obj->GetActive() && !obj->IsDeleted())
	{
		obj->SetPosition(this->x, this->y);
		obj->SetActive(true);
		obj->OnEnable();
	}
}

void RespawnPoint::Delete()
{
	if (obj != nullptr) obj = nullptr;
	GameObject::Delete();
}
#pragma endregion