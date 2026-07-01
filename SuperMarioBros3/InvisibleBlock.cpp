#include "InvisibleBlock.h"
#include "NoteBlock.h"
#include "QuestionBlock.h"
#include "PlayScene.h" // Để lấy Scene hiện tại và thêm Block mới vào danh sách

InvisibleBlock::InvisibleBlock(float x, float y, InvisibleType type) : GameObject(x, y)
{
	this->type = type;
	this->isTriggered = false;
}

void InvisibleBlock::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	if (isTriggered) return;

	l = x - 8.0f;
	t = y - 8.0f;
	r = x + 8.0f;
	b = y + 8.0f;
}

void InvisibleBlock::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	// Nếu đã kích hoạt, logic biến hình sẽ được xử lý ở đây hoặc bên phía Mario
	if (isTriggered)
	{
		this->isDeleted = true; // Đánh dấu xóa khỏi danh sách object hiện tại
	}
}

void InvisibleBlock::Render()
{
	RenderBoundingBox(); 
}

void InvisibleBlock::Trigger()
{
	if (isTriggered) return;
	isTriggered = true;

	PlayScene* scene = (PlayScene*)SceneManager::GetInstance()->GetCurrentScene();
	GameObject* newBlock = nullptr;

	if (type == InvisibleType::NOTE_BLOCK)
	{
		newBlock = new NoteBlock(x, y, true);
	}
	else if (type == InvisibleType::QUESTION_BLOCK)
	{
		newBlock = new QuestionBlock(x, y);
	}

	if (newBlock != nullptr)
	{
		scene->AddObject(newBlock);
		DebugOut(L"[TEST] Da chay qua AddObject!\n");
	}
}