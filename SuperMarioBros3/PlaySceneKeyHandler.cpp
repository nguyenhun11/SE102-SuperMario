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
	case DIK_S:
		//mario->SetState(MarioState::SIT);
		mario->EnterPipeDown();
		break;
	case DIK_Z:
	case DIK_SPACE:
		mario->SetState(MarioState::JUMP);
		mario->IsHoldingJump = true; // 
		break;
	case DIK_UP:
		mario->EnterPipeUp();
		break;
	case DIK_W:
	case DIK_X:
		mario->Attack();
		break;
	/// begin Test Mario Form
	case DIK_1:
		mario->SetNewForm(MarioForm::SMALL);
		break;
	case DIK_2:
		mario->SetNewForm(MarioForm::SUPER);
		break;
	case DIK_3:
		mario->SetNewForm(MarioForm::RACOON);
		break;
	case DIK_0:
		mario->SetState(MarioState::DIE);
		break;
	/// end Test Mario Form

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
	case DIK_Z:
	case DIK_SPACE:
		mario->SetState(MarioState::JUMP_RELEASE);
		mario->IsHoldingJump = false;
		break;
	case DIK_DOWN:
	case DIK_S:
		mario->SetState(MarioState::SIT_RELEASE);
		break;
	}
}

void PlaySceneKeyHandler::KeyState(BYTE *states)
{
	InputManager* input = InputManager::GetInstance();
	Mario* mario = (Mario*)((LPPLAYSCENE)SceneManager::GetInstance()->GetCurrentScene())->GetPlayer();
	if (mario == NULL) return;

	if (input->IsKeyDown(DIK_UP))
	{
		mario->EnterPipeUp();
		return; 
	}
	else if (input->IsKeyDown(DIK_DOWN) || input->IsKeyDown(DIK_S))
	{
		mario->EnterPipeDown();
		return;
	}

	if (input->IsKeyDown(DIK_RIGHT) || input->IsKeyDown(DIK_D))
	{
		mario->SetDirection(1);
		if (input->IsKeyDown(DIK_X) || input->IsKeyDown(DIK_W))
			mario->SetState(MarioState::RUNNING);
		else
			mario->SetState(MarioState::WALKING);
	}
	else if (input->IsKeyDown(DIK_LEFT) || input->IsKeyDown(DIK_A))
	{
		mario->SetDirection(-1);
		if (input->IsKeyDown(DIK_X) || input->IsKeyDown(DIK_W))
			mario->SetState(MarioState::RUNNING);
		else
			mario->SetState(MarioState::WALKING);
	}
	else
		mario->SetState(MarioState::IDLE);
}