#pragma once
#include <unordered_map>

#include "Singleton.h"    
#include "Scene.h"
#include "InputManager.h"
#include "Sprites.h"
#include "Animations.h"

using namespace std;

#define ID_SPRITE_BLACK_SCREEN 9999999
#define FADE_SPEED 1.5f

class SceneManager : public Singleton<SceneManager>
{
	friend class Singleton<SceneManager>;
private:
	unordered_map<int, LPSCENE> scenes;
	int current_scene = -1;
	int next_scene = -1;
public:
	LPSCENE GetCurrentScene() { return scenes[current_scene]; }
	void SwitchScene();
	void InitiateSwitchScene(int scene_id);
	void AddScene(int scene_id, LPSCENE scene);
};


