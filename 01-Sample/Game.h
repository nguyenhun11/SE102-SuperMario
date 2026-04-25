#pragma once
#include <Windows.h>
#include <d3d10.h>
#include <d3dx10.h>
#include <unordered_map>
#include <string>
#include "Singleton.h"

#include "Texture.h"
#include "KeyEventHandler.h"
#include "Scene.h"

#include "SceneManager.h"
#include "InputManager.h"
#include "Camera.h"

using namespace std;



/*
	Our simple game framework
*/
class Game : public Singleton<Game>
{
public:
	void Init(HWND hWnd, HINSTANCE hInstance);
	~Game();

	void Update(DWORD dt);
	void Render();

	void Load(LPCWSTR gameFile);
	void _ParseSection_SETTINGS(string line);
	void _ParseSection_SCENES(string line);
	void _ParseSection_TEXTURES(string line);

	void Draw(float x, float y, LPTEXTURE tex, RECT* rect = NULL, float alpha = 1.0f, int sprite_width = 0, int sprite_height = 0);

	void Draw(float x, float y, LPTEXTURE tex, int l, int t, int r, int b, float alpha = 1.0f, int sprite_width = 0, int sprite_height = 0)
	{
		RECT rect;
		rect.left = l;
		rect.top = t;
		rect.right = r;
		rect.bottom = b;
		this->Draw(x, y, tex, &rect, alpha, sprite_width, sprite_height);
	}

	LPTEXTURE LoadTexture(LPCWSTR texturePath);
	void SetPointSamplerState();
};
typedef Game* LPGAME;