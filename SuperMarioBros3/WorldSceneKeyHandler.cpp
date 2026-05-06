#include "WorldSceneKeyHandler.h"
#include "debug.h"
#include "Game.h"
#include "MapMario.h" // Nhớ include đúng tên class MapMario của Hưn
#include "WorldScene.h"

void WorldSceneKeyHandler::OnKeyDown(int KeyCode)
{
}

void WorldSceneKeyHandler::OnKeyUp(int KeyCode)
{
}

void WorldSceneKeyHandler::KeyState(BYTE* states)
{
	WorldScene* scene = (WorldScene*)SceneManager::GetInstance()->GetCurrentScene();
	MapMario* mario = (MapMario*)scene->GetPlayer();
	if (mario == NULL) return;

	InputManager* input = InputManager::GetInstance();

	if (mario->IsMoving()) return;

	if (input->IsKeyDown(DIK_RIGHT) || input->IsKeyDown(DIK_D))
		mario->MoveRight();
	else if (input->IsKeyDown(DIK_LEFT) || input->IsKeyDown(DIK_A))
		mario->MoveLeft();
	else if (input->IsKeyDown(DIK_UP) || input->IsKeyDown(DIK_W))
		mario->MoveUp();
	else if (input->IsKeyDown(DIK_DOWN) || input->IsKeyDown(DIK_S))
		mario->MoveDown();
}