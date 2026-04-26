#include "Sprite.h"
#include "Game.h"

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

//void Sprite::Draw(float x, float y, bool isFlip = false)
//{
//	//Game* g = Game::GetInstance();
//	float cx, cy;
//	Camera::GetInstance()->GetCamPos(cx, cy);
//
//	cx = (FLOAT)floor(cx);
//	cy = (FLOAT)floor(cy);
//
//	D3DXMATRIX matTranslation;
//	
//	x = (FLOAT)floor(x);
//	y = (FLOAT)floor(y);
//
//	D3DXMatrixTranslation(&matTranslation, x - cx, GameGlobal::GetHeight() - y + cy, 0.1f);
//
//	this->sprite.matWorld = (this->matScaling * matTranslation);
//
//	GameGlobal::spriteObject->DrawSpritesImmediate(&sprite, 1, 0, 0);
//}

void Sprite::Draw(float x, float y, bool isFlip)
{
	float cx, cy;
	Camera::GetInstance()->GetCamPos(cx, cy);

	cx = (FLOAT)floor(cx);
	cy = (FLOAT)floor(cy);
	x = (FLOAT)floor(x);
	y = (FLOAT)floor(y);

	// Tạo một ma trận scale cục bộ
	D3DXMATRIX matFinalScaling = this->matScaling;
	D3DXMATRIX matTranslation;

	// XỬ LÝ LẬT HÌNH
	if (isFlip)
	{
		int spriteWidth = (this->right - this->left + 1);
		int spriteHeight = (this->bottom - this->top + 1);

		D3DXMatrixScaling(&matFinalScaling, (FLOAT)-spriteWidth, (FLOAT)spriteHeight, 1.0f);
		x += spriteWidth;
	}

	D3DXMatrixTranslation(&matTranslation, x - cx, GameGlobal::GetHeight() - y + cy, 0.1f);
	this->sprite.matWorld = (matFinalScaling * matTranslation);
	GameGlobal::spriteObject->DrawSpritesImmediate(&sprite, 1, 0, 0);
}

