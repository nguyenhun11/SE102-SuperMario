#pragma once
#include "SceneKeyHandler.h"

class WorldSceneKeyHandler : public SceneKeyHandler 
{
	virtual void KeyState(BYTE* states);
	virtual void OnKeyDown(int KeyCode);
	virtual void OnKeyUp(int KeyCode);
	WorldSceneKeyHandler(LPSCENE s) : SceneKeyHandler(s) {};
};