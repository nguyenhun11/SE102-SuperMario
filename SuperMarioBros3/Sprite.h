#pragma once

#include "Texture.h"

class Sprite
{
	int id;				// Sprite ID in the sprite database

	int left;
	int top;
	int right;
	int bottom;

	LPTEXTURE texture;
	D3DX10_SPRITE sprite;
	D3DXMATRIX matScaling;
	void Draw(float x, float y, float cx, float cy, bool isFlip, bool isFlipVertical);
public:
	Sprite(int id, int left, int top, int right, int bottom, LPTEXTURE tex);

	int GetWidth() { return right - left + 1; }
	int GetHeight() { return bottom - top + 1; }

	void DrawOnCamera(float x, float y, bool isFlip = false, bool isFlipVertical = false);
	void DrawOnScreen(float x, float y);
};

typedef Sprite* LPSPRITE;