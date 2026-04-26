#include "SceneManager.h"

void SceneManager::InitiateSwitchScene(int scene_id)
{
	DebugOut(L"[SceneManager] Nhan duoc lenh InitiateSwitchScene voi ID = %d\n", scene_id);
	next_scene = scene_id;
}

void SceneManager::AddScene(int scene_id, LPSCENE scene)
{
	scenes[scene_id] = scene;
}

void SceneManager::SwitchScene()
{
	if (next_scene < 0 || next_scene == current_scene) return;

	DebugOut(L"[INFO] Switching to scene %d\n", next_scene);

	if (scenes[current_scene] != NULL)
		scenes[current_scene]->Unload();

	Sprites::GetInstance()->Clear();
	Animations::GetInstance()->Clear();

	current_scene = next_scene;
	LPSCENE s = scenes[next_scene];
	InputManager::GetInstance()->SetKeyHandler(s->GetKeyEventHandler());
	s->Load();
}