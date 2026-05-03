#include "Sprite.h"
#include "Game.h"

void Sprite::Draw(float x, float y, float cx, float cy, bool isFlip)
{
	x = (FLOAT)floor(x);
	y = (FLOAT)floor(y);
	cx = (FLOAT)floor(cx);
	cy = (FLOAT)floor(cy);

	D3DXMATRIX matTranslation;
	D3DXMatrixTranslation(&matTranslation, x - cx, GameGlobal::GetHeight() - y + cy, 0.1f);

	this->sprite.matWorld = (this->matScaling * matTranslation);

	float texWidth = (float)this->texture->getWidth();
	int spriteWidth = (this->right - this->left + 1);

	if (isFlip)
	{
		this->sprite.TexCoord.x = (this->right + 1.0f) / texWidth;
		this->sprite.TexSize.x = -(float)spriteWidth / texWidth;
	}
	else
	{
		this->sprite.TexCoord.x = this->left / texWidth;
		this->sprite.TexSize.x = (float)spriteWidth / texWidth;
	}

	GameGlobal::spriteObject->DrawSpritesImmediate(&sprite, 1, 0, 0);
}

Sprite::Sprite(int id, int left, int top, int right, int bottom, LPTEXTURE tex)
{
	this->id = id;
	this->left = left;
	this->top = top;
	this->right = right;
	this->bottom = bottom;
	this->texture = tex;

	float texWidth = (float)tex->getWidth();
	float texHeight = (float)tex->getHeight();

	// Set the sprite’s shader resource view
	sprite.pTexture = tex->getShaderResourceView();

	sprite.TexCoord.x = this->left / texWidth;
	sprite.TexCoord.y = this->top / texHeight;

	int spriteWidth = (this->right - this->left + 1);
	int spriteHeight = (this->bottom - this->top + 1);

	sprite.TexSize.x = spriteWidth / texWidth;
	sprite.TexSize.y = spriteHeight / texHeight;

	sprite.ColorModulate = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	sprite.TextureIndex = 0;

	D3DXMatrixScaling(&this->matScaling, (FLOAT)spriteWidth, (FLOAT)spriteHeight, 1.0f);
}

void Sprite::DrawOnCamera(float x, float y, bool isFlip)
{
	float cx, cy;
	Camera::GetInstance()->GetCamPos(cx, cy);
	Draw(x, y, cx, cy, isFlip);
}

void Sprite::DrawOnScreen(float x, float y)
{
	Draw(x, y, 0.0f, 0.0f, false);
}

