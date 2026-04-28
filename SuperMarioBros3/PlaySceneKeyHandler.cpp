#include "PlaySceneKeyHandler.h"

#include "debug.h"
#include "Game.h"

#include "Mario.h"
#include "PlayScene.h"

void PlaySceneKeyHandler::OnKeyDown(int KeyCode)
{
	//DebugOut(L"[INFO] KeyDown: %d\n", KeyCode);
	Mario* mario = (Mario *)((LPPLAYSCENE)SceneManager::GetInstance()->GetCurrentScene())->GetPlayer(); 

	switch (KeyCode)
	{
	case DIK_DOWN:
		mario->SetState(MarioState::SIT);
		break;
	case DIK_S:
		mario->SetState(MarioState::JUMP);
		mario->IsHoldingJump = true; // 
		break;
	case DIK_1:
		mario->SetNewForm(MarioForm::SMALL);
		break;
	case DIK_2:
		mario->SetNewForm(MarioForm::SUPER);
		break;
	case DIK_0:
		mario->SetState(MarioState::DIE);
		break;
	case DIK_R: // reset
		//Reload();
		break;
	}
}

void PlaySceneKeyHandler::OnKeyUp(int KeyCode)
{
	//DebugOut(L"[INFO] KeyUp: %d\n", KeyCode);

	Mario* mario = (Mario*)((LPPLAYSCENE)SceneManager::GetInstance()->GetCurrentScene())->GetPlayer();
	if (mario == NULL) return;

	switch (KeyCode)
	{
	case DIK_S:
		mario->SetState(MarioState::JUMP_RELEASE);
		mario->IsHoldingJump = false;
		break;
	case DIK_DOWN:
		mario->SetState(MarioState::SIT_RELEASE);
		break;
	}
}

void PlaySceneKeyHandler::KeyState(BYTE *states)
{
	InputManager* input = InputManager::GetInstance();
	Mario* mario = (Mario*)((LPPLAYSCENE)SceneManager::GetInstance()->GetCurrentScene())->GetPlayer();
	if (mario == NULL) return;

	if (input->IsKeyDown(DIK_RIGHT))
	{
		mario->SetDirection(1);
		if (input->IsKeyDown(DIK_A))
			mario->SetState(MarioState::RUNNING);
		else
			mario->SetState(MarioState::WALKING);
	}
	else if (input->IsKeyDown(DIK_LEFT))
	{
		mario->SetDirection(-1);
		if (input->IsKeyDown(DIK_A))
			mario->SetState(MarioState::RUNNING);
		else
			mario->SetState(MarioState::WALKING);
	}
	else
		mario->SetState(MarioState::IDLE);
}