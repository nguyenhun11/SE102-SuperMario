#pragma once

#include "KeyEventHandler.h"
#include <string>

/*
*  Abstract class for a game scene
*/
class Scene
{
protected:
	LPKEYEVENTHANDLER key_handler;
	int id;
	LPCWSTR sceneFilePath;
	float backColor[4];
	std::string sceneBGM;

public: 
	Scene(int id, LPCWSTR filePath)
	{
		this->id = id;
		this->sceneFilePath = filePath;
		this->key_handler = NULL;

		//default color cho playscene
		backColor[0] = 181.0f / 255; // R
		backColor[1] = 235.0f / 255; // G
		backColor[2] = 242.0f / 255; // B
		backColor[3] = 0.0f;         // A
	}

	LPKEYEVENTHANDLER GetKeyEventHandler() { return key_handler; }
	virtual void Load() = 0;
	virtual void Unload() = 0;
	virtual void Update(DWORD dt) = 0;
	virtual void Render() = 0; 

	void SetBackgroundColor(float r, float g, float b, float a = 0.0f) {
		backColor[0] = r; backColor[1] = g; backColor[2] = b; backColor[3] = a;
	}
	int GetId() { return id; }

	const float* GetBackgroundColor() { return backColor; }
};
typedef Scene * LPSCENE;


