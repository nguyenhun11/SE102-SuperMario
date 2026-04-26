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
public:
	Sprite(int id, int left, int top, int right, int bottom, LPTEXTURE tex);

	void Draw(float x, float y);
};

typedef Sprite* LPSPRITE;